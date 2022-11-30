#include "utils.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"

#define SHOW_NORMALS true

// Width and height should be 1920 x 1080.
// Temporarily set to 1000 and 700 in order to display on smaller laptop screens.
const int WIDTH = 1000;
const int HEIGHT = 700;
GLFWwindow *window;

// Initialize camera
Camera camera(vec3(0.0f, 0.0f, 8.0f));


void process_mouse_func([[maybe_unused]] GLFWwindow *win, double x, double y) {
    camera.process_mouse(x, y);
}

void process_scroll_func([[maybe_unused]] GLFWwindow *win, [[maybe_unused]] double x, double y) {
    camera.process_scroll(y);
}

int main() {
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw1");
    glEnable(GL_DEPTH_TEST);

    // Input callback function
    glfwSetCursorPosCallback(window, process_mouse_func);         // mouse cursor movement callback
    glfwSetScrollCallback(window, process_scroll_func);           // mouse scroll callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // disable cursor

    // create mesh from file
    Mesh bunny("assets/bunny.obj");
    Mesh plane("assets/plane.obj");
    Mesh sphere("assets/sphere.obj");

    // Create and compile the object's shader itself (without geometry shader)
    Shader objShader("src/vertexShader.glsl", "src/fragShader.glsl");
#if SHOW_NORMALS
    // Create and compile the normal vertex shader (with geometry shader)
    Shader normalShader("src/geo_vertexShader.glsl", "src/geo_fragShader.glsl", "src/geo_geometryShader.glsl");
#endif
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // objShader.setVec3("light_pos", glm::vec3(5.0f, 7.0f, 8.0f));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        camera.update_time();  // update time to ensure same movement speed
        camera.process_wasd(window);  // process keyboard WASD inputs

        // use object shader
        objShader.use();
        objShader.setVec3("light_pos_world_1", vec3(5.0f, 7.0f, 8.0f));
        objShader.setVec3("light_pos_world_2", vec3(-3.0f, 2.0f, -3.0f));
        objShader.setMat4("model", mat4(1.0f));
        objShader.setMat4("view", camera.get_view());
        objShader.setMat4("proj",
                          glm::perspective(radians(camera.fov), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f));
        objShader.setVec3("spot_direction", camera.m_gaze);
        objShader.setVec3("spot_position", camera.m_position);
        bunny.draw();  // draw object

#if SHOW_NORMALS
        // use normal shader to display normal vectors
        normalShader.use();
        normalShader.setMat4("model", glm::mat4(1.0f));
        normalShader.setMat4("view", camera.get_view());
        normalShader.setMat4("proj",
                             glm::perspective(radians(camera.fov), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f));
        bunny.draw();  // draw the object again (this time with normals)
#endif

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}