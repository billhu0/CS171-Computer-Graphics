#include "utils.h"
#include "camera.h"
#include "enum.h"
#include "object.h"
#include "shader.h"
#include "bezier.h"
#include "bspline.h"
#include <chrono>

#define PERFORMANCE_TEST  // measure execution time of important parts
#define PERFORMANCE_MARK(MESSAGE) \
    do { \
        std::clog << \
        std::chrono::duration_cast<std::chrono::milliseconds> \
        (std::chrono::system_clock::now() - curr_time).count() \
        << "ms: " << MESSAGE << '\n' << std::flush; \
        curr_time = std::chrono::system_clock::now(); \
    } while (0)

const int WIDTH = 800;
const int HEIGHT = 600;
GLFWwindow *window;

// Initialize camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));

void process_mouse_func([[maybe_unused]] GLFWwindow *win, double x, double y) {
    camera.process_mouse(x, y);
}

void process_scroll_func([[maybe_unused]] GLFWwindow *win, [[maybe_unused]] double x, double y) {
    camera.process_scroll(y);
}

BETTER_ENUM(RenderCase, int,
            uniform_single_bezier_surface,
            uniform_multi_bezier_surface,
            single_spline_surface,
            adaptive_single_bezier_surface,
            adaptive_multi_bezier_surface)
/**
 * BETTER_ENUM is from https://aantron.github.io/better-enums/
 * You can add any render case as you like,
 * this is to allow you to demonstrate multi task in single executable file.
 * */

RenderCase choice = RenderCase::_values()[0];

/**
 * Choose a rendering case in the terminal.
 * */
void choose_render_case() {
    for (size_t index = 0; index < RenderCase::_size(); ++index) {
        RenderCase render_case = RenderCase::_values()[index];
        std::cout << index << ". " << +render_case << '\n';
    }
    while (true) {
        std::cout << "choose a rendering case from [0:" << RenderCase::_size() - 1 << "]" << '\n';
        std::string input;
        std::cin >> input;
        if (isNumber(input) && std::stoi(input) >= 0 && std::stoi(input) < (int) RenderCase::_size()) {
            choice = RenderCase::_values()[std::stoi(input)];
            break;
        } else {
            std::cout << "Wrong input.\n";
        }
    } 
}

/**
 * Testcases
 * */
void testcases(Object& obj, std::vector<Object>& objects) {
    #ifdef PERFORMANCE_TEST
    auto curr_time = std::chrono::system_clock::now();
    #endif

    // generate mesh
    switch (choice) {
        case RenderCase::uniform_single_bezier_surface: {
            std::cout << "You chose " << +RenderCase::uniform_single_bezier_surface << '\n';

            BezierSurface bezier(std::vector<std::vector<vec3>> {
                {vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 6.0f, 0.0f), vec3(0.0f, 9.0f, 0.0f)},
                {vec3(3.0f, 0.0f, 0.0f), vec3(3.0f, 3.0f, 9.0f), vec3(3.0f, 6.0f, 0.0f), vec3(3.0f, 9.0f, 0.0f)},
                {vec3(6.0f, 0.0f, 0.0f), vec3(6.0f, 3.0f, 0.0f), vec3(6.0f, 6.0f, -9.0f), vec3(6.0f, 9.0f, 0.0f)},
                {vec3(9.0f, 0.0f, 0.0f), vec3(9.0f, 3.0f, 0.0f), vec3(9.0f, 6.0f, 0.0f), vec3(9.0f, 9.0f, 0.0f)}
            });

            obj = bezier.generateObject();
            obj.init();
            break;
        }
        case RenderCase::uniform_multi_bezier_surface: {
            std::cout << "You chose " << +RenderCase::uniform_multi_bezier_surface << '\n';
            PERFORMANCE_MARK("Chosen uniform_multi_bezier_surface");

            std::vector<BezierSurface> bezier_surfaces = read("assets/tea.bzs");
            PERFORMANCE_MARK("Bezier Surface Reading consumed");

            for (BezierSurface& bezier_surface : bezier_surfaces) {
                obj = bezier_surface.generateObject();
                obj.init();
                objects.push_back(obj);
            }
            PERFORMANCE_MARK("Object generation consumed");
            break;
        }
        case RenderCase::single_spline_surface: {
            std::cout << "You chose " << +RenderCase::single_spline_surface << '\n';

            BSplineSurface bs_surface(std::vector<std::vector<vec3>> {
                {vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 6.0f, 0.0f), vec3(0.0f, 9.0f, 0.0f), vec3(0.0f, 12.0f, 0.0f)},
                {vec3(3.0f, 0.0f, 0.0f), vec3(3.0f, 3.0f, 20.0f), vec3(3.0f, 6.0f, 0.0f), vec3(3.0f, 9.0f, 0.0f), vec3(3.0f, 12.0f, 0.0f)},
                {vec3(6.0f, 0.0f, 0.0f), vec3(6.0f, 3.0f, 0.0f), vec3(6.0f, 6.0f, 30.0f), vec3(6.0f, 9.0f, 0.0f), vec3(6.0f, 12.0f, 0.0f)},
                {vec3(9.0f, 0.0f, 0.0f), vec3(9.0f, 3.0f, 0.0f), vec3(9.0f, 6.0f, 0.0f), vec3(9.0f, 9.0f, -20.0f), vec3(9.0f, 12.0f, 0.0f)},
                {vec3(12.0f, 0.0f, 0.0f), vec3(12.0f, 3.0f, 0.0f), vec3(12.0f, 6.0f, 0.0f), vec3(12.0f, 9.0f, 0.0f), vec3(12.0f, 12.0f, 0.0f)}
            }, 4);
            PERFORMANCE_MARK("B-spline init consumed");

            obj = bs_surface.generateObject();
            obj.init();
            PERFORMANCE_MARK("B-spline object generation and init consumed");
            break;
        }
        case RenderCase::adaptive_single_bezier_surface : {
            std::cout << "You chose " << +RenderCase::adaptive_single_bezier_surface << '\n';

            BezierSurface bezier(std::vector<std::vector<vec3>> {
                {vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 6.0f, 0.0f), vec3(0.0f, 9.0f, 0.0f)},
                {vec3(3.0f, 0.0f, 0.0f), vec3(3.0f, 3.0f, 9.0f), vec3(3.0f, 6.0f, 0.0f), vec3(3.0f, 9.0f, 0.0f)},
                {vec3(6.0f, 0.0f, 0.0f), vec3(6.0f, 3.0f, 0.0f), vec3(6.0f, 6.0f, -9.0f), vec3(6.0f, 9.0f, 0.0f)},
                {vec3(9.0f, 0.0f, 0.0f), vec3(9.0f, 3.0f, 0.0f), vec3(9.0f, 6.0f, 0.0f), vec3(9.0f, 9.0f, 0.0f)}
            });

            obj = bezier.generateObjectAdaptive();
            obj.init();
            break;
        }
        case RenderCase::adaptive_multi_bezier_surface: {
            std::cout << "You chose " << +RenderCase::adaptive_multi_bezier_surface << '\n';
            PERFORMANCE_MARK("Chosen adaptive_multi_bezier_surface");

            std::vector<BezierSurface> bezier_surfaces = read("assets/tea.bzs");
            PERFORMANCE_MARK("Bezier surfaces read consumed");

            for (BezierSurface& bezier_surface : bezier_surfaces) {
                obj = bezier_surface.generateObjectAdaptive();
                obj.init();
                objects.push_back(obj);
            }
            PERFORMANCE_MARK("Object generation consumed");

            break;
        }
    }
}

int main() {
    // choose render case
    choose_render_case();

    WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS171 hw2");
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, process_mouse_func);
    glfwSetScrollCallback(window, process_scroll_func);
    glEnable(GL_DEPTH_TEST);

    Object obj;  // single object
    std::vector<Object> objects;  // multiple objects
    testcases(obj, objects);

    // create and compile shaders
    Shader shader("src/shader_vertex.glsl", "src/shader_frag.glsl");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update_time();
        camera.process_keyboard(window);

        shader.use();
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setMat4("view", camera.get_view());
        shader.setMat4("proj", camera.get_proj());
        shader.setVec3("spot_direction", camera.m_gaze);
        shader.setVec3("spot_position", camera.m_position);

        switch (choice) {
            // single surface
            case RenderCase::uniform_single_bezier_surface:
            case RenderCase::single_spline_surface:
            case RenderCase::adaptive_single_bezier_surface: {
                obj.drawElements();
                break;
            }
            // multi surface
            case RenderCase::uniform_multi_bezier_surface:
            case RenderCase::adaptive_multi_bezier_surface: {
                for (Object & object: objects) object.drawElements();
                break;
            }
        }
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

