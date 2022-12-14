#include "bsdf.h"
#include "utils.h"


Vec3f IdealDiffusion::evaluate(Interaction &interaction) const {
    return color / PI;
}

float IdealDiffusion::pdf(Interaction &interaction) const {
    // cos-theta / pi
    return interaction.normal.dot(interaction.wi) / PI;
}

Vec3f IdealDiffusion::sample(Interaction &interaction, Sampler &sampler) const {
    // Get a uniform distribution sample in [0,1]^2
    Vec2f sample = sampler.get2D();

    // Transform the sample into a point on the hemisphere
    const float theta = std::acos(std::sqrt(1.f - sample.y()));
    const float phi = 2.f * PI * sample.x();

    // Polar coordinate on the sphere should be
    // (sin theta * cos phi, sin theta * sin phi, cos theta)
    Vec3f hemisphere_wi = Vec3f(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));

    Mat3f transform = Eigen::Quaternionf::FromTwoVectors(Vec3f{0.f, 0.f, 1.f}, interaction.normal).toRotationMatrix();
    interaction.wi = (transform * hemisphere_wi).normalized();

    return interaction.wi;
}

// return whether the bsdf is perfect transparent or perfect reflection
bool IdealDiffusion::isDelta() const {
    return false;
}

// return whether the bsdf is perfect transparent or perfect reflection
bool IdealSpecular::isDelta() const {
    return true;
}
