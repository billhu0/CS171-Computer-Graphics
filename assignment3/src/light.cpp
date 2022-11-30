#include "light.h"

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) : position(pos), color(color) {}

Vec3f Light::getColor() const { return color; }

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension, const Vec3f &normal,
                                 const Vec3f &tangent)
    : Light(pos, color), rectangle(pos, dimension, normal, tangent) {}

std::vector<LightSample> SquareAreaLight::samples() const {
    std::vector<LightSample> samples;
    // TODO: Your code here.

    // image width and height
    const float width = rectangle.getSize().x();
    const float height = rectangle.getSize().y();

    const Vec3f center = rectangle.getPos();  // light center
    const Vec3f tangent = rectangle.getTangent();  // light tangent
    const Vec3f cotangent = rectangle.getNormal().cross(tangent);  // light cotangent

    for (unsigned int i = 0; i < SAMPLE_NUM; i++) {
        for (unsigned int j = 0; j < SAMPLE_NUM; j++) {

            // Normalize i, j to [-1, 1].
            // The first sample gets -1, the last sample gets 1, the other linearly distributed.
            const float ratio_x = 2.0f * ((float)i + 0.5f) / SAMPLE_NUM - 1;
            const float ratio_y = 2.0f * ((float)j + 0.5f) / SAMPLE_NUM - 1;

            // Map the above ratio to the image width and height
            const float x = ratio_x * width / 2.0f;
            const float y = ratio_y * height / 2.0f;

            // Calculate sample position.
            const Vec3f sample_pos = center + tangent * x + cotangent * y;
            samples.push_back(LightSample{color, sample_pos});  // color = light color

        }
    }

    return samples;
}
bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
    // TODO: Your code here.

    // To check intersection with light, simply check intersection with the rectangle.
    if (rectangle.intersect(ray, interaction)) {
        // If intersection happens, replace the interaction type with 'LIGHT'.
        interaction.type = Interaction::LIGHT;
        return true;
    }

    // Otherwise, no intersection.
    return false;
}
