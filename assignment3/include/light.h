#ifndef CS171_HW3_INCLUDE_LIGHT_H_
#define CS171_HW3_INCLUDE_LIGHT_H_

#include <vector>

#include "core.h"
#include "ray.h"
#include "geometry.h"

struct LightSample {
    Vec3f color;
    Vec3f position;
};

class Light {
   public:
    explicit Light(const Vec3f &pos, const Vec3f &color);
    virtual ~Light() = default;
    [[nodiscard]] virtual std::vector<LightSample> samples() const = 0;
    virtual bool intersect(Ray &ray, Interaction &interaction) const = 0;

    [[nodiscard]] Vec3f getColor() const;

   protected:
    Vec3f position;  // position of light in world space
    Vec3f color;  // RGB color of the light
};

class SquareAreaLight : public Light {
   public:
    explicit SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &dimension, const Vec3f &normal,
                             const Vec3f &tangent);
    [[nodiscard]] std::vector<LightSample> samples() const override;
    bool intersect(Ray &ray, Interaction &interaction) const override;

   protected:
    Rectangle rectangle;
};

#endif  // CS171_HW3_INCLUDE_LIGHT_H_
