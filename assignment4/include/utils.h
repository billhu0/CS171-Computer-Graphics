#ifndef UTILS_H_
#define UTILS_H_

#include "core.h"

#include <random>

namespace utils {

static inline float clamp01(float v) {
    if (v > 1)
        v = 1;
    else if (v < 0)
        v = 0;
    return v;
}

static inline uint8_t gammaCorrection(float radiance) {
    return static_cast<uint8_t>(255.f * clamp01(powf(radiance, 1.f / 2.2f)));
}

static inline float radians(float x) { return x * PI / 180; }

static inline Vec3f deNan(const Vec3f& vec, float val) {
    Vec3f tmp = vec;
    if (vec.x() != vec.x()) tmp.x() = val;
    if (vec.y() != vec.y()) tmp.y() = val;
    if (vec.z() != vec.z()) tmp.z() = val;
    return tmp;
}

// A function to check if a radiance color vector is valid.
static bool is_invalid(const Vec3f& v) {
    return (std::isnan(v.x()) || std::isnan(v.y()) || std::isnan(v.z()) ||  // isnan
            std::isinf(v.x()) || std::isinf(v.y()) || std::isinf(v.z()) ||  // isinf
            v.x() > 1.f || v.y() > 1.f || v.z() > 1.f);                 // > 1
}

}  // namespace utils

class Sampler {
   public:
    Sampler() = default;
    float get1D() { return dis(engine); }
    Vec2f get2D() { return {dis(engine), dis(engine)}; }
    void setSeed(int i) { engine.seed(i); }

   private:
    std::default_random_engine engine;
    std::uniform_real_distribution<float> dis;
};

#endif  // UTILS_H_
