#ifndef CS171_HW3_INCLUDE_UTILS_H_
#define CS171_HW3_INCLUDE_UTILS_H_

#include "core.h"

namespace utils {

static inline float clamp01(float v) {
    if (v > 1) v = 1;
    else if (v < 0) v = 0;
    return v;
}

static inline uint8_t gammaCorrection(float radiance) {
    return static_cast<uint8_t>(ceil(255 * pow(clamp01(radiance), 1 / 2.2)));
}

// Halton sequence.
// This function is used to generate points in (0,1) x (0,1) R^2.
static float halton_sequence(int index, int base) {
    float f = 1;
    float r = 0;
    while (index > 0) {
        f = f / (float) base;
        r = r + f * (float) (index % base);
        index = index / base;
    }
    return r;
}

}  // namespace utils

#endif  // CS171_HW3_INCLUDE_UTILS_H_
