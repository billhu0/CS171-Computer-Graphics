//
// A 'Camera' class with camera position, facing, pitch and so on
// in order to make camera controls easier
//

#pragma once

#include "glm/glm.hpp"
#include "utils.h"

class Camera {

   public:
    vec3 m_position = vec3(0.0f, 0.0f, 0.0f);;
    vec3 m_gaze = vec3(0.0f, 0.0f, -1.0f);
    vec3 m_up = vec3(0.0f, 1.0f, 0.0f);
    mat4 m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    double m_pitch = 0, m_yaw = -90;

    // Camera object constructors
    Camera() noexcept = default;

    explicit Camera(const vec3 &position) noexcept {
        m_position = position;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    // camera movements
    void move_up(float d) {
        m_position += m_up * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void move_down(float d) {
        m_position -= m_up * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void move_left(float d) {
        m_position -= glm::normalize(glm::cross(m_gaze, m_up)) * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void move_right(float d) {
        m_position += glm::normalize(glm::cross(m_gaze, m_up)) * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void move_front(float d) {
        m_position += m_gaze * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void move_back(float d) {
        m_position -= m_gaze * d;
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    void rotate(double delta_x, double delta_y) {
        m_pitch += delta_y;
        if (m_pitch > 89) m_pitch = 89;
        else if (m_pitch < -89) m_pitch = -89;
        m_yaw += delta_x;

        vec3 gaze = {
            cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw)),
            sin(glm::radians(m_pitch)),
            cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw))
        };
        m_gaze = glm::normalize(gaze);
        m_view = glm::lookAt(m_position, m_position + m_gaze, m_up);
    }

    [[nodiscard]] mat4 get_view() const {
        return m_view;
    }

    // process input functions
   private:
    double delta_time = 0, last_time = 0;
    double last_x = 0, last_y = 0;
    bool first_mouse = true;

   public:
    float fov = 45;  // field of view 视野

    void update_time() {
        auto curr_time = glfwGetTime();
        delta_time = curr_time - last_time;
        last_time = curr_time;
    }

    void process_wasd(GLFWwindow *win) {
        float camera_speed = 5.0f * delta_time;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
            move_up(camera_speed);
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
            move_down(camera_speed);
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
            move_left(camera_speed);
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
            move_right(camera_speed);
        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS)
            move_front(camera_speed);
        if (glfwGetKey(win, GLFW_KEY_F) == GLFW_PRESS)
            move_back(camera_speed);
    }

    void process_mouse(double x, double y) {
        if (first_mouse) {
            last_x = x;
            last_y = y;
            first_mouse = false;
        }
        double mouse_speed = 0.1;
        rotate((x - last_x) * mouse_speed, (last_y - y) * mouse_speed);
        last_x = x;
        last_y = y;
    }

    void process_scroll(double y_offset) {
        if (fov >= 1.0f && fov <= 45.0f) fov -= (float) y_offset;
        if (fov <= 1.0f) fov = 1.0f;
        if (fov >= 45.0f) fov = 45.0f;
    }
};
