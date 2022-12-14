#include "cloth.h"
#include "object.h"
#include "transform.h"
#include "input.h"
#include "camera.h"

///////////////////
/// constructor ///
///////////////////

RectCloth::RectCloth(float cloth_weight, const UVec2& mass_dim, float dx_local, float stiffness, float damping_ratio)
    : Mesh(std::vector<MeshVertex>(mass_dim.x * mass_dim.y),
           std::vector<UVec3>((mass_dim.y - 1) * (mass_dim.x - 1) * 2), GL_STREAM_DRAW, GL_STATIC_DRAW, true),
      // private
      mass_dim(mass_dim),
      mass_weight(cloth_weight / float(mass_dim.x * mass_dim.y)),
      dx_local(dx_local),
      stiffness(stiffness),
      damping_ratio(damping_ratio),
      is_fixed_masses(mass_dim.x * mass_dim.y),
      local_or_world_positions(mass_dim.x * mass_dim.y),
      world_velocities(mass_dim.x * mass_dim.y),
      world_accelerations(mass_dim.x * mass_dim.y) {
    // initialize local positions
    const auto local_width = float(mass_dim.x) * dx_local;
    const auto local_height = float(mass_dim.y) * dx_local;

    #pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y; ++ih) {
        for (int iw = 0; iw < mass_dim.x; ++iw) {
            local_or_world_positions[Get1DIndex(iw, ih)] = 
                Vec3(float(iw) * dx_local - local_width * 0.5f, float(ih) * dx_local - local_height * 0.5f, 0);
        }
    }
    // initialize mesh vertices
    UpdateMeshVertices();

    // initialize mesh indices
    #pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y - 1; ++ih) {
        for (int iw = 0; iw < mass_dim.x - 1; ++iw) {
            size_t i_indices = (size_t(ih) * size_t(mass_dim.x - 1) + size_t(iw)) << 1;

            auto i = Get1DIndex(iw, ih);
            auto r = Get1DIndex(iw + 1, ih);
            auto u = Get1DIndex(iw, ih + 1);
            auto ru = Get1DIndex(iw + 1, ih + 1);

            indices[i_indices] = UVec3(i, r, u);
            indices[i_indices + 1] = UVec3(r, ru, u);
        }
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UVec3) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);

#ifdef SIMULATE_WIND
    std::cerr << "\033[37;45m Wind force simulation is on! \033[0m\n";
#else
    std::cerr << " \033[37;46m Wind force simulation is off! \033[0m\n";
#endif
}

//////////////////
/// interfaces ///
//////////////////

bool RectCloth::SetMassFixedOrNot(int iw, int ih, bool fixed_or_not) {
    iw = iw < 0 ? int(mass_dim.x) + iw : iw;
    ih = ih < 0 ? int(mass_dim.y) + ih : ih;

    size_t idx;
    if (!Get1DIndex(iw, ih, idx)) return false;

    is_fixed_masses[idx] = fixed_or_not;
    return true;
}

/*override*/ void RectCloth::FixedUpdate() {
    // simulate
    Simulate(simulation_steps_per_fixed_update_time);

    // update mesh vertices
    UpdateMeshVertices();
}

/////////////////////////
/// force computation ///
/////////////////////////

Vec3 RectCloth::ComputeHookeForce(int iw_this, int ih_this, int iw_that, int ih_that, float dx_world) const {
    /*! TODO: compute the force according to Hooke's law
     *        applied to mass(iw_this, ih_this) by mass(iw_that, ih_that).
     *        `dx_world` is "the zero-force distance" in world coordinate
     *
     *        note: for invalid `iw` or `ih`, simply return { 0, 0, 0 }
     */

    // check if "iw_this", "ih_this" is invalid
    if (iw_this < 0 || iw_this >= mass_dim.x || ih_this < 0 || ih_this >= mass_dim.y) return {0, 0, 0};
    // check if "iw_that", "ih_that" is invalid
    if (iw_that < 0 || iw_that >= mass_dim.x || ih_that < 0 || ih_that >= mass_dim.y) return {0, 0, 0};

    // distance between two positions
    const Vec3 distance = local_or_world_positions[Get1DIndex(iw_this, ih_this)] - local_or_world_positions[Get1DIndex(iw_that, ih_that)];

    // Force = kx
    return stiffness * (dx_world - glm::length(distance)) * glm::normalize(distance);
}

Vec3 RectCloth::ComputeSpringForce(int iw, int ih) const {
    const Vec3 scale = object->transform->scale;

    /*! TODO: compute the total spring force applied to mass(iw, ih)
     *        by some other neighboring masses
     *
     *        note: you MUST consider structural, shear, and bending springs
     *              you MUST consider scaling of "the zero-force distance"
     *              you may find ComputeHookeForce() helpful
     *              for invalid `iw` or `ih`, you may simply return { 0, 0, 0 }
     *              for "fixed masses", you may also simply return { 0, 0, 0 }
     */


    if (is_fixed_masses[Get1DIndex(iw, ih)]){
        return {0, 0, 0};
    }

    Vec3 force = {0, 0, 0};

    // structural springs
    force += ComputeHookeForce(iw, ih, iw - 1, ih, dx_local * scale.x);
    force += ComputeHookeForce(iw, ih, iw + 1, ih, dx_local * scale.x);
    force += ComputeHookeForce(iw, ih, iw, ih - 1, dx_local * scale.y);
    force += ComputeHookeForce(iw, ih, iw, ih + 1, dx_local * scale.y);

    // shear springs
    force += ComputeHookeForce(iw, ih, iw - 1, ih - 1, std::sqrt(2.f) * dx_local);
    force += ComputeHookeForce(iw, ih, iw + 1, ih - 1, std::sqrt(2.f) * dx_local);
    force += ComputeHookeForce(iw, ih, iw - 1, ih + 1, std::sqrt(2.f) * dx_local);
    force += ComputeHookeForce(iw, ih, iw + 1, ih + 1, std::sqrt(2.f) * dx_local);

    // bending springs
    force += ComputeHookeForce(iw, ih, iw - 2, ih, dx_local * 2);
    force += ComputeHookeForce(iw, ih, iw + 2, ih, dx_local * 2);
    force += ComputeHookeForce(iw, ih, iw, ih - 2, dx_local * 2);
    force += ComputeHookeForce(iw, ih, iw, ih + 2, dx_local * 2);

    force -= damping_ratio * world_velocities[Get1DIndex(iw, ih)];
    return force;
}

///////////////////////////
/// simulation pipeline ///
///////////////////////////

void RectCloth::LocalToWorldPositions() {
    const Mat4 model_matrix = object->transform->ModelMat();

    /*! TODO: implement this: transform mass positions from local coordinate to world coordinate
     *
     *        note: you may find `class Transform` in `transform.h` helpful
     */

    // For each position vector in 'local_or_world_positions', multiply it by model_matrix
    std::for_each(local_or_world_positions.begin(), local_or_world_positions.end(), [&](Vec3 & position) {
        position = model_matrix * Vec4(position, 1);
    });
}



void RectCloth::ComputeAccelerations() {
    /*! TODO: implement this: compute accelerations for each mass
     *
     *        note: you may find ComputeSpringForce() helpful
     *              you may store the results into `world_accelerations`
     */

    #ifdef SIMULATE_WIND

    // wind intensity calculation
    static int count = 0;
    static Vec3 wind = {0, 0, distribution(e) / 50};
    if (count > 100) {
        count = 0;
        wind = {0, 0, distribution(e) / 50};
    } else {
        count += 1;
    }

    #endif

    for (int i = 0; i < mass_dim.x; i++) {
        for (int j = 0; j < mass_dim.y; j++) {
            if (is_fixed_masses[Get1DIndex(i, j)]) continue;
            #ifdef SIMULATE_WIND
            world_accelerations[Get1DIndex(i, j)] = (ComputeSpringForce(i, j) + wind) / mass_weight - gravity;
            #else 
            world_accelerations[Get1DIndex(i, j)] = (ComputeSpringForce(i, j)) / mass_weight - gravity;
            #endif
        }
    }
}

void RectCloth::ComputeVelocities() {
    /*! TODO: implement this: update velocities for each mass
     *
     *        note: you may store the results into `world_velocities`
     *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
     */
    for (size_t i = 0; i < world_velocities.size(); ++i) {
        if (is_fixed_masses[i]) continue;
        world_velocities[i] += world_accelerations[i] * fixed_delta_time;
    }

    // handle mouse drag event

    // When a new mouse click happens, calculate the mass it is pointing at.
    if (!mouse_pressed && Input::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << "\033[37;41m Mouse pressed \033[0m"; 
        mouse_pressed = true;

        // get mouse position and transform it from screen coordinate to world coordinate
        Vec3 look_at = glm::unProject(Input::mouse_position, camera->LookAtMat(), camera->PerspectiveMat(), Vec4{0, 0, 640, 480});
        
        // shot a ray at this direction 
        Vec3 ray_dir = glm::normalize(look_at - camera->transform.position);

        // shot a ray at this direction and find the nearest mass.
        // The ray origin is 'camera->transform.position'. Ray direction is 'ray_dir'.
        float min_distance = std::numeric_limits<float>::max();
        int min_index = -1;
        for (size_t i = 0; i < local_or_world_positions.size(); ++i) {
            // sphere center: local_or_world_positions[i]
            // ray o: camera->transform.position;  ray d: ray_dir

            // calculate distance
            const float a = glm::dot(local_or_world_positions[i] - camera->transform.position, ray_dir);
            const float distance = std::sqrt(glm::length(local_or_world_positions[i] - camera->transform.position) * glm::length(local_or_world_positions[i] - camera->transform.position) - a * a);

            if (distance < 0.05f && distance < min_distance) {
                min_index = i;
                min_distance = distance;
            }
        }

        if (min_index != -1) {
            drag_index = min_index;
            world_velocities[drag_index] = Vec3{0,0,0};
            
            fprintf(stderr, " \033[37;43m Mouse pressed on mass number %d (Row %d Col %d). \033[0m", min_index, min_index % mass_dim.x, min_index / mass_dim.x);

        } else {
            fprintf(stderr, " \033[37;43m Mouse pressed on empty space. \033[0m");
        }
    } 

    // mouse is pressed and moving (maintaining the fixed mass)
    else if (mouse_pressed && Input::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        // transform mouse position from screen coordinate to world coordinate
        Vec3 look_at = glm::unProject(Input::mouse_position, camera->LookAtMat(), camera->PerspectiveMat(), Vec4{0, 0, 640, 480});
        
        // shot a ray at this direction and find the nearest mass
        Vec3 ray_dir = glm::normalize(look_at - camera->transform.position);

        // plane parameters. The plane is perpendicular to the camera forward direction and goes through the mass point.
        Vec3 normal = camera->transform.Forward();
        Vec3 x0 = local_or_world_positions[drag_index];

        // do the ray-plane intersection
        float t = (glm::dot(normal, x0) - glm::dot(normal, camera->transform.position)) / glm::dot(normal, ray_dir);
        
        // calculate the new position and set the mass position to it.
        local_or_world_positions[drag_index] = camera->transform.position + t * ray_dir;
        world_velocities[drag_index] = Vec3{0, 0, 0};
    }
    
    else if (mouse_pressed && !Input::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
        std::cerr << " \033[37;42m Mouse released \033[0m\n";

        drag_index = -1;
        mouse_pressed = false;
    }
}

void RectCloth::ComputePositions() {
    /*! TODO: implement this: update positions for each mass
     *
     *        note: you may store the results into `local_or_world_positions`
     *              you may use `this->fixed_delta_time` instead of `Time::fixed_delta_time`, why?
     */
    for (size_t i = 0; i < local_or_world_positions.size(); ++i) {
        if (is_fixed_masses[i]) continue;
        local_or_world_positions[i] += world_velocities[i] * fixed_delta_time;

        // collision detection
        const Vec3 distance = local_or_world_positions[i] - sphere_center;
        if (glm::length(distance) <= sphere_radius) {
            const Vec3 normal = glm::normalize(distance);
            local_or_world_positions[i] = sphere_center + normal * sphere_radius;  // update position
            world_velocities[i] += glm::dot(-world_velocities[i], normal) * normal;  // add 'velocity along normal'
        }
    }
}

void RectCloth::WorldToLocalPositions() {
    const Mat4 model_matrix = object->transform->ModelMat();

    /*! TODO: implement this: transform mass positions from world coordinate to local coordinate
     *
     *        note: you may find `class Transform` in `transform.h` helpful
     */
    
    const Mat4 inverse_model_matrix = glm::inverse(model_matrix);
    std::for_each(local_or_world_positions.begin(), local_or_world_positions.end(), [&](Vec3 & position) {
        position = inverse_model_matrix * Vec4(position, 1);
    });
}

void RectCloth::Simulate(unsigned num_steps) {
    for (unsigned i = 0; i < num_steps; ++i) {
        LocalToWorldPositions();
        ComputeAccelerations();
        ComputeVelocities();
        ComputePositions();
        WorldToLocalPositions();
    }
}

/////////////////
/// rendering ///
/////////////////

void RectCloth::UpdateMeshVertices() {
    // set vertex positions
    for (size_t i = 0; i < local_or_world_positions.size(); ++i) vertices[i].position = local_or_world_positions[i];

    // reset vertex normals
    auto compute_normal = [&](auto v1, auto v2, auto v3) {
        return glm::normalize(
            glm::cross(vertices[v2].position - vertices[v1].position, vertices[v3].position - vertices[v1].position));
    };

    #pragma omp parallel for collapse(2)
    for (int ih = 0; ih < mass_dim.y; ++ih)
        for (int iw = 0; iw < mass_dim.x; ++iw) {
            constexpr float w_small = 0.125f;
            constexpr float w_large = 0.25f;

            auto i = Get1DIndex(iw, ih);
            auto l = Get1DIndex(iw - 1, ih);
            auto r = Get1DIndex(iw + 1, ih);
            auto u = Get1DIndex(iw, ih + 1);
            auto d = Get1DIndex(iw, ih - 1);
            auto lu = Get1DIndex(iw - 1, ih + 1);
            auto rd = Get1DIndex(iw + 1, ih - 1);
            auto& normal = vertices[i].normal;

            normal = {0, 0, 0};

            if (iw > 0 && ih < mass_dim.y - 1) {
                normal += compute_normal(l, i, lu) * w_small;
                normal += compute_normal(i, u, lu) * w_small;
            }
            if (iw < mass_dim.x - 1 && ih < mass_dim.y - 1) {
                normal += compute_normal(i, r, u) * w_large;
            }
            if (iw > 0 && ih > 0) {
                normal += compute_normal(l, d, i) * w_large;
            }
            if (iw < mass_dim.x - 1 && ih > 0) {
                normal += compute_normal(d, rd, i) * w_small;
                normal += compute_normal(rd, r, i) * w_small;
            }

            normal = glm::normalize(normal);
        }

    // vbo
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), vertices.data(), buffer_data_usage_vbo);
    glBindVertexArray(0);
}

//////////////////////////
/// supporting methods ///
//////////////////////////

size_t RectCloth::Get1DIndex(int iw, int ih) const {
    return size_t(ih) * size_t(mass_dim.x) + size_t(iw);
}

bool RectCloth::Get1DIndex(int iw, int ih, size_t& idx) const {
    if (iw < 0 || ih < 0 || iw >= mass_dim.x || ih >= mass_dim.y) return false;
    idx = size_t(ih) * size_t(mass_dim.x) + size_t(iw);
    return true;
}
