#ifndef BSDF_H_
#define BSDF_H_

#include "interaction.h"

class BSDF {
   public:
    BSDF() = default;
    virtual ~BSDF() = default;
    [[nodiscard]] virtual Vec3f evaluate(Interaction &interaction) const = 0;
    virtual float pdf(Interaction &interaction) const = 0;
    virtual Vec3f sample(Interaction &interaction, Sampler &sampler) const = 0;
    [[nodiscard]] virtual bool isDelta() const = 0;
};

// Ideal diffusion surface
class IdealDiffusion : public BSDF {
   public:
    explicit IdealDiffusion(Vec3f color) : color(std::move(color)) {}
    [[nodiscard]] Vec3f evaluate(Interaction &interaction) const override;
    float pdf(Interaction &interaction) const override;
    Vec3f sample(Interaction &interaction, Sampler &sampler) const override;
    [[nodiscard]] bool isDelta() const override;  // return false

   private:
    Vec3f color;
};

// You can add your own BSDF here

// Ideal Specular (mirror, reflection) surface
class IdealSpecular : public BSDF {
   public:
    explicit IdealSpecular(Vec3f color) : color(std::move(color)) {}

    [[nodiscard]] Vec3f evaluate(Interaction &interaction) const override {
        return {0.f, 0.f, 0.f};
    };

    float pdf(Interaction &interaction) const override {
        return 0;
    };

    Vec3f sample(Interaction &interaction, Sampler &sampler) const override {
        return {0.f, 0.f, 0.f};
    };

    [[nodiscard]] bool isDelta() const override;  // return true

   private:
    Vec3f color;
};

#endif  // BSDF_H_
