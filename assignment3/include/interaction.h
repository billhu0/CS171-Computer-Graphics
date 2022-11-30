#ifndef CS171_HW3_INCLUDE_INTERACTION_H_
#define CS171_HW3_INCLUDE_INTERACTION_H_

#include "core.h"

struct PhongModel {
    Vec3f diffusion;
    Vec3f specular;
    Vec3f ambient;
    float shininess{};
};

struct Interaction {
    enum Type { LIGHT, GEOMETRY, NONE } type = NONE;
    Vec3f pos{};  // the position where intersection occurs
    float dist{RAY_DEFAULT_MAX};  // the ray's t value when intersection occurs
    Vec3f normal{};
    Vec2f uv{};  // for texture mapping use
    Vec3f T, B, N;  // for normal texture use
    PhongModel model;
};

#endif  // CS171_HW3_INCLUDE_INTERACTION_H_
