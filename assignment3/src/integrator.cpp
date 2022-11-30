#include "integrator.h"
#include "utils.h"
#include <omp.h>
#include <utility>


PhongLightingIntegrator::PhongLightingIntegrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene)
    : camera(std::move(cam)), scene(std::move(scene)) {}

void PhongLightingIntegrator::render() const {
    const int img_x = camera->getImage()->getResolution().x();
    const int img_y = camera->getImage()->getResolution().y();

    // Preprocess: generate halton sequence.
    // For each pixel (dx, dy), store the halton points it contains into the following vector<vector<vector<sample>>>.
    std::vector<std::vector<std::vector<Vec2f>>> samples_in_pixel;
    samples_in_pixel.resize(img_x);
    for (int i = 0; i < img_x; i++) {
        samples_in_pixel[i].resize(img_y);
    }

    // For each halton point, we need calculate which pixel it belongs to.
    const int num_haltons = img_x * img_y * 16;
    for (int i = 0; i < num_haltons; i++) {
        
        // halton sample coordinate
        Vec2f halton_sample = {
            (float) img_x * utils::halton_sequence(i, 2),
            (float) img_y * utils::halton_sequence(i, 3)
        };

        // this sample belongs which pixel?
        samples_in_pixel[(int) halton_sample.x()][(int) halton_sample.y()].push_back(halton_sample);

    }


    int cnt = 0;
#pragma omp parallel for schedule(dynamic) shared(cnt)
    for (int dx = 0; dx < img_x; dx++) {
#pragma omp atomic
        ++cnt;
        printf("\r%.02f%%", cnt * 100.0 / img_x);
        for (int dy = 0; dy < img_y; dy++) {
            // TODO: Your code here.

            /* With super-sampling and magic angle anti-aliasing. */
             Vec3f L(0, 0, 0);
             std::vector<Vec2f> ray_samples = camera->getRaySamples((float) dx, (float) dy, 4);
             for (auto &ray_sample: ray_samples) {
                 Ray ray = camera->generateRay(ray_sample.x(), ray_sample.y());
                 Interaction interaction;
                 if (scene->intersect(ray, interaction)) {
                     L += radiance(ray, interaction) / (float) ray_samples.size();
                 }
             }
             camera->getImage()->setPixel(dx, dy, L);
            /* With anti-aliasing. */

            /* With halton sequence */
//            Vec3f L(0, 0, 0);
//            for (auto &halton_sample : samples_in_pixel[dx][dy]) {
//                Ray ray = camera->generateRay(halton_sample.x(), halton_sample.y());
//                Interaction interaction;
//                if (scene->intersect(ray, interaction)) {
//                    L += radiance(ray, interaction) / (float) samples_in_pixel[dx][dy].size();
//                }
//            }
//            camera->getImage()->setPixel(dx, dy, L);
            /* With halton sequence */

            /* No anti-aliasing */
//             Vec3f L(0, 0, 0);
//             Ray ray = camera->generateRay(dx, dy);
//             Interaction interaction;
//             if (scene->intersect(ray, interaction)) {
//                 L = radiance(ray, interaction);
//             }
//             camera->getImage()->setPixel(dx, dy, L);
            /* No anti-aliasing */

        }
    }
}

Vec3f PhongLightingIntegrator::radiance(Ray &ray, Interaction &interaction) const {
    Vec3f radiance(0, 0, 0);
    // TODO: Your code here.

    // No interaction. Radiance is black.
    if (interaction.type == Interaction::Type::NONE) {
        radiance = Vec3f(0, 0, 0);
        return radiance;
    }

    // Interaction with light. Radiance is light color.
    if (interaction.type == Interaction::Type::LIGHT) {
        radiance = scene->getLight()->getColor();
        return radiance;
    }

    // Otherwise (interaction is geometry), apply phong lighting model to calculate color.
    Vec3f ambient = scene->getAmbient().cwiseProduct(interaction.model.ambient);
    Vec3f diffuse(0, 0, 0);
    Vec3f specular(0, 0, 0);

    std::vector<LightSample> light_samples = scene->getLight()->samples();

    // traverse every light sample
    for (auto &light_sample: light_samples) {

        // calculate shadow ray
        Vec3f light_dir = (light_sample.position - interaction.pos).normalized();
        Ray shadowRay(interaction.pos, light_dir);

        // check if shadow ray is blocked
        if (!scene->isShadowed(shadowRay)) {
            // diffusion coefficient
            float diff = std::max(0.0f, light_dir.dot(interaction.normal));
            diffuse += diff * interaction.model.diffusion.cwiseProduct(light_sample.color);

            // specular coefficient
            Vec3f reflectDir = (2 * interaction.normal.dot(light_dir) * interaction.normal - light_dir).normalized();
            float spec = std::pow(std::max(0.0f, reflectDir.dot(-ray.direction)), interaction.model.shininess);
            specular += spec * interaction.model.specular.cwiseProduct(light_sample.color);
        }
    }

    radiance = ambient + (diffuse + specular) / light_samples.size();
    return radiance;
}
