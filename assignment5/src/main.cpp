#include "cloth.h"
#include "scene.h"

int main() {
    // settings

    // window
    constexpr int window_width = 640;
    constexpr int window_height = 480;

    // cloth
    constexpr float cloth_weight = 2.0f;
    constexpr UVec2 mass_dim = {40, 30};
    constexpr float dx_local = 0.1f;
    constexpr float stiffness = 15.f;
    constexpr float damping_ratio = 0.0015f;
    std::vector<IVec2> fixed_masses{{0, -1}, {-1, -1}};

    // setup window
    GLFWwindow* window;
    if (!glfwInit())  // initialize glfw library
        return -1;

    // setting glfw window hints and global configurations
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // use core mode
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "CS171 HW5: Cloth Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    // load Opengl
    if (!gladLoadGL()) {
        glfwTerminate();
        return -1;
    }

    // setup call back functions
    glfwSetFramebufferSizeCallback(window, Input::CallBackResizeFlareBuffer);

    // main Loop
    {
        // shader
        Shader::Initialize();

        // scene
        Scene scene(45);
        scene.camera.transform.position = {0, -1.5, -6};
        scene.camera.transform.rotation = {0, 0, 1, 0};
        scene.light_position = {0, 3, -10};
        scene.light_color = Vec3(1, 1, 1) * 1.125f;

        // clothes
        auto cloth = std::make_shared<RectCloth>(cloth_weight, mass_dim, dx_local, stiffness, damping_ratio);
        for (const auto& fixed_mass : fixed_masses) {
            if (!cloth->SetMassFixedOrNot(fixed_mass.x, fixed_mass.y, true)) abort();
        }
        // store a reference of the camera class to RectCloth
        cloth->camera = &scene.camera;

        // mesh primitives
        auto mesh_cube = std::make_shared<Mesh>(MeshPrimitiveType::cube);
        auto mesh_sphere = std::make_shared<Mesh>(MeshPrimitiveType::sphere);

        // objects
        auto object_cloth = scene.AddObject(
            cloth, Shader::shader_phong,
            Transform(Vec3(0, 0, 0),
                      glm::quat_cast(glm::rotate(Mat4(float(1.0)), glm::radians(float(60)), Vec3(1, 0, 0))),
                      Vec3(1, 1, 1)));
        auto object_cube = scene.AddObject(mesh_cube, Shader::shader_phong,
                                           Transform(Vec3(-3.5, -1.8, 0.3), Quat(1, 0, 0, 0), Vec3(1, 1, 1)));
        auto object_sphere = scene.AddObject(mesh_sphere, Shader::shader_phong,
                                             Transform(Vec3(0, -1.0, 0.3), Quat(1, 0, 0, 0), Vec3(1, 1, 1)));
        object_cloth->color = {0, 0.75, 1};
        object_cube->color = {0.75, 1, 0};
        object_sphere->color = {1, 0.75, 0};

        // loop until the user closes the window
        Input::Start(window);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        while (!glfwWindowShouldClose(window)) {
            Input::Update();
            Time::Update();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // terminate
            if (Input::GetKey(KeyCode::Escape)) glfwSetWindowShouldClose(window, true);

            // fixed update
            for (unsigned i = 0; i < Time::fixed_update_times_this_frame; ++i) {
                if (Input::GetKey(KeyCode::Space)) {
                    scene.FixedUpdate();
                }
            }

            // update
            scene.Update();

            // render
            scene.RenderUpdate();

            glfwSwapBuffers(window);  // swap front and back buffers
            glfwPollEvents();  // poll for and process events
        }
    }

    glfwTerminate();
    return 0;
}
