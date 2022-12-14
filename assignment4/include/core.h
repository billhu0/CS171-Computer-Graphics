#ifndef CORE_H_
#define CORE_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <memory>
#include <vector>
#include <stack>

#define UNIMPLEMENTED

using Vec3f = Eigen::Vector3f;
using Vec3i = Eigen::Vector3i;
using Vec2f = Eigen::Vector2f;
using Vec2i = Eigen::Vector2i;
using Vec4f = Eigen::Vector4f;
using Mat3f = Eigen::Matrix3f;
using Mat2f = Eigen::Matrix2f;
using Mat3d = Eigen::Matrix3d;

constexpr float RAY_DEFAULT_MIN = 1e-5;
constexpr float RAY_DEFAULT_MAX = 1e7;
constexpr float PI = 3.141592653579f;

class BSDF;
class Sampler;

#define USE_GLOBAL_BVH  // Turn on to use BVH acceleration
#define USE_LINEARIZED_BVH  // Turn on to use compressed linear BVH

#endif  // CORE_H_
