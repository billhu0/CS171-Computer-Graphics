#include "integrator.h"
#include "utils.h"
#include <omp.h>

#include <utility>

Integrator::Integrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene, int spp, int max_depth)
    : camera(std::move(cam)),
      scene(std::move(scene)),
      spp(spp),
      spp_sqrt(static_cast<int>(std::sqrt(spp))),
      max_depth(max_depth) {
}

void Integrator::render() const {
    Vec2i resolution = camera->getImage()->getResolution();
    int cnt = 0;
    Sampler sampler;

    #ifdef USE_ROTATED_GRID
    // rotated grid
    const float magic_angle = std::atan(0.5f);
    Mat2f rotator;
    rotator << std::sin(magic_angle), -std::cos(magic_angle), std::cos(magic_angle), std::sin(magic_angle);
    #endif

    #pragma omp parallel for schedule(dynamic), shared(cnt, resolution), private(sampler), default(none)
    for (int dx = 0; dx < resolution.x(); dx++) {
        #pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / resolution.x());
        sampler.setSeed(omp_get_thread_num());

        for (int dy = 0; dy < resolution.y(); dy++) {
            Vec3f L(0, 0, 0);
            // Generate #spp rays for each pixel and use Monte Carlo integration to compute radiance.

            // Generate pixel samples for pixel (dx, dy)
            for (int i = 0; i < spp_sqrt; i++) {
                for (int j = 0; j < spp_sqrt; j++) {
                    float x = ((float)i + 0.5f) / (float)spp_sqrt;
                    float y = ((float)j + 0.5f) / (float)spp_sqrt;
                    
                    #ifdef USE_ROTATED_GRID
                    Vec2f sample = rotator * Vec2f(x, y) + Vec2f(0.5f + (float)dx, 0.5f + (float)dy);
                    Ray ray = camera->generateRay(sample.x(), sample.y());
                    #else 
                    Ray ray = camera->generateRay(x + dx, y + dy);
                    #endif
                    L += radiance(ray, sampler, 0);
                }
            }

            camera->getImage()->setPixel(dx, dy, L / spp);
        }
    }
}

Vec3f Integrator::radiance(Ray &ray, Sampler &sampler, int depth) const {

    // If max depth exceeded, return zero
    if (depth >= max_depth) {
        return {0.f, 0.f, 0.f};
    }

    // Check intersection with the scene. If no intersection, return zero.
    Interaction interaction;
    if (!scene->intersect(ray, interaction)) {
        return {0.f, 0.f, 0.f};
    }

    // There is an intersection. Check intersection type.
    switch (interaction.type) {
        
        // No intersection. Return zero.
        case Interaction::NONE: {
            return {0.f, 0.f, 0.f};
        }

        // Intersection with light. Directly get light emission color.
        case Interaction::LIGHT: {
            return scene->getLight()->emission(interaction.normal, ray.direction);
        }

        // Intersection with geometry. Calculate direct light and recursively calculate indirect light.
        case Interaction::GEOMETRY: {
            // Calculate direct light.
            Vec3f directLight = directLighting(interaction, sampler);

            // Calculate indirect light.
            Vec3f indirectLight(0, 0, 0);
            interaction.wo = -ray.direction;

            // ideal diffusion
            if (!interaction.material->isDelta()) {
                Vec3f wi = interaction.material->sample(interaction, sampler);
                Ray nextRay(interaction.pos, wi);
                if (!scene->isShadowed(nextRay)) {
                    return directLight;
                }
                Vec3f L = radiance(nextRay, sampler, depth + 1);
                indirectLight = interaction.material->evaluate(interaction).cwiseProduct(L) *
                                wi.dot(interaction.normal) / interaction.material->pdf(interaction);
            }

            // ideal specular (mirror)
            else {
                Vec3f wi = -interaction.wo + 2 * (interaction.wo.dot(interaction.normal)) * interaction.normal;
                Ray nextRay(interaction.pos, wi);
                if (!scene->isShadowed(nextRay)) {
                    return directLight;
                }
                indirectLight = radiance(nextRay, sampler, depth + 1);
            }

            return directLight + indirectLight;
        }

        // (unreachable code)
        default: {
            return {0.f, 0.f, 0.f};
        }
    }
}

Vec3f Integrator::directLighting(Interaction &interaction, Sampler &sampler) const {
    Vec3f L(0, 0, 0);
    // Compute direct lighting.

    Vec3f sample_pos = scene->getLight()->sample(interaction, nullptr, sampler);
    Vec3f ray_dir = sample_pos - interaction.pos;
    Ray shadowRay(interaction.pos, ray_dir);

    if (!scene->isShadowed(shadowRay)) {
        float cos_theta_i = interaction.normal.dot(ray_dir.normalized());
        float cos_theta_o = scene->getLight()->getNormal().dot(-ray_dir.normalized());

        L = cos_theta_i * cos_theta_o *
            scene->getLight()->emission(sample_pos, ray_dir).cwiseProduct(interaction.material->evaluate(interaction));
        L /= (float)std::pow((sample_pos - interaction.pos).norm(), 2);
        L /= scene->getLight()->pdf(interaction, sample_pos);
    }

    return L;
}
