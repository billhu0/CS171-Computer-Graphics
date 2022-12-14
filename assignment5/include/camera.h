#pragma once

#include "input.h"
#include "time_system.h"
#include "transform.h"

// #define FIRST_PERSON_CAMERA

class Camera {
   public:
    // transform
    Transform transform = Transform();

    // perspective
    float fov_y;
    float aspect = 1;
    float near = 0.1f;
    float far = 100.0f;

    // move
    float mouse_sensitivity = 0.085f;

    #ifdef FIRST_PERSON_CAMERA
    // first person camera
    Float pitch_max = Float(75.0);
    #endif

    explicit Camera(float fov_y);

    Camera(const Camera&) = default;
    Camera(Camera&&) = default;
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;
    ~Camera() = default;

    [[nodiscard]] Mat4 LookAtMat() const;
    [[nodiscard]] Mat4 PerspectiveMat() const;

    void Update();

   private:
    float speeding_rate = zero;
    bool is_speeding = false;

    static constexpr float MOVE_SPEED_SLOW = 3.0f;
    static constexpr float MOVE_SPEED_FAST = 6.0f;
    static constexpr float ACCELERATION = 2.5f;
    static constexpr float DECELERATION = 8.0f;
};
