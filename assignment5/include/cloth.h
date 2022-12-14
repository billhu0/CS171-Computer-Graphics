#pragma once

#include "mesh.h"
#include "time_system.h"

#define SIMULATE_WIND

class RectCloth : public Mesh {
   public:
    // constructor
    RectCloth(float cloth_weight, const UVec2& mass_dim, float dx_local, float stiffness, float damping_ratio);
    RectCloth(const RectCloth&) = default;
    RectCloth(RectCloth&&) = default;
    RectCloth& operator=(const RectCloth&) = default;
    RectCloth& operator=(RectCloth&&) = default;
    ~RectCloth() override = default;

    // interfaces
    bool SetMassFixedOrNot(int iw, int ih, bool fixed_or_not);
    void FixedUpdate() override;

   private:
    static constexpr unsigned simulation_steps_per_fixed_update_time = 20;
    static constexpr float fixed_delta_time = Time::fixed_delta_time / float(simulation_steps_per_fixed_update_time);

    UVec2 mass_dim;
    float mass_weight;

    float dx_local;

    float stiffness;
    float damping_ratio;

    std::vector<bool> is_fixed_masses;
    std::vector<Vec3> local_or_world_positions;
    std::vector<Vec3> world_velocities;
    std::vector<Vec3> world_accelerations;

    // force computation
    [[nodiscard]] Vec3 ComputeHookeForce(int iw_this, int ih_this, int iw_that, int ih_that, float dx_world) const;
    [[nodiscard]] Vec3 ComputeSpringForce(int iw, int ih) const;

    // simulation pipeline
    void LocalToWorldPositions();
    void ComputeAccelerations();
    void ComputeVelocities();
    void ComputePositions();
    void WorldToLocalPositions();
    void Simulate(unsigned num_steps);

    // rendering
    void UpdateMeshVertices();

    // supporting methods
    [[nodiscard]] size_t Get1DIndex(int iw, int ih) const;
    bool Get1DIndex(int iw, int ih, size_t& idx) const;

    // random distribution for wind generation (bonus 1)
    std::default_random_engine e;
    std::uniform_real_distribution<float> distribution;

    // sphere collision detection (bonus 2)
    Vec3 sphere_center = Vec3(0, -1.0, 0.3);
    float sphere_radius = 0.52f;

    // mouse interaction (bonus 3)
    bool mouse_pressed = false;
    int drag_index = -1;

   public:
    Camera* camera;

};
