#pragma once

// platform

#define ADVISE_INLINE inline

#if defined(__CUDA_ARCH__)
  #define ALWAYS_INLINE __forceinline__
#else
  #if defined(_MSC_VER) || defined(__INTEL_COMPILER)
    #define ALWAYS_INLINE __forceinline
  #else
    #define ALWAYS_INLINE __attribute__((always_inline)) inline
  #endif
#endif

// fwd

// input
class Input;
// time
class Time;
// object
class Object;
// mesh
enum class MeshPrimitiveType;
class MeshVertex;
class Mesh;
// shader
class Shader;
// transform
class Transform;
// camera
class Camera;
// scene
class Scene;
// cloth
class RectCloth;

// includes

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C/C++
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// definitions
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Quat = glm::quat;
using Mat2 = glm::mat2;
using Mat3 = glm::mat3;
using Mat4 = glm::mat4;
using IVec2 = glm::ivec2;
using IVec3 = glm::ivec3;
using UVec2 = glm::uvec2;
using UVec3 = glm::uvec3;

// constants

constexpr float zero = 0.f;
constexpr float one = 1.f;
constexpr float pi = std::numbers::pi_v<float>;
constexpr float inv_pi = std::numbers::inv_pi_v<float>;
constexpr float inf = std::numeric_limits<float>::infinity();
constexpr float epsilon = std::numeric_limits<float>::epsilon();

constexpr Vec3 gravity = {0, 9.8, 0};

// utility functions

std::string GetFilePath(const std::string& target, int depth = 5);
