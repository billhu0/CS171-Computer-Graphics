#include <iostream>
#include <chrono>

#include "integrator.h"

#ifndef TESTBYGITHUB  // PLEASE DO NOT CHANGE
int scene_id = 1;
#else                 // PLEASE DO NOT CHANGE
int scene_id = 0;     // PLEASE DO NOT CHANGE
#endif                // TESTBYGITHUB

void setSceneById(std::shared_ptr<Scene> &scene, int id);

int main() {

    setbuf(stdout, nullptr);

#ifndef TESTBYGITHUB                 // PLEASE DO NOT CHANGE
    Vec2i img_resolution(400, 400);
#else                                // PLEASE DO NOT CHANGE
    Vec2i img_resolution(32, 32);    // PLEASE DO NOT CHANGE
#endif                               // TESTBYGITHUB

    // init image.
    std::shared_ptr<ImageRGB> rendered_img = std::make_shared<ImageRGB>(img_resolution.x(), img_resolution.y());

    // camera setting
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    camera->setPosition(Vec3f(0, -3, 1.3));
    camera->lookAt(Vec3f(0, 0, 1));
    camera->setFov(45);
    camera->setImage(rendered_img);

    // construct scene.
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    setSceneById(scene, scene_id);

    // render scene
    PhongLightingIntegrator integrator(camera, scene);
    std::cout << "Start Rendering...\n";
    auto start = std::chrono::steady_clock::now();

    integrator.render();
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\nRender Finished in " << time << "ms.\n";

#ifndef TESTBYGITHUB                                // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("../result.png");
#else                                               // PLEASE DO NOT CHANGE
    rendered_img->writeImgToFile("../submit.png");  // PLEASE DO NOT CHANGE
#endif                                              // TESTBYGITHUB

    std::cout << "Image saved to disk.\n";
    return 0;
}

void setSceneById(std::shared_ptr<Scene> &scene, int id) {
    Vec3f light_pos(0, 0, 1.95f);
    // Vec3f light_color(1, 1, 1);
    Vec3f light_color(17.0,12.0,5.0);
    light_color.normalize();
    Vec2f light_dim(0.5, 0.3);
    Vec3f light_n(0, 0, -1);
    Vec3f light_t(1, 0, 0);
    auto square_light = std::make_shared<SquareAreaLight>(light_pos, light_color, light_dim, light_n, light_t);

    // Geometries 
    auto floor = std::make_shared<Rectangle>(Vec3f(0, 0, 0), Vec2f(2, 2), Vec3f(0, 0, 1), Vec3f(1, 0, 0));
    auto ceiling = std::make_shared<Rectangle>(Vec3f(0, 0, 2), Vec2f(2, 2), Vec3f(0, 0, -1), Vec3f(1, 0, 0));
    auto wall_left = std::make_shared<Rectangle>(Vec3f(-1, 0, 1), Vec2f(2, 2), Vec3f(1, 0, 0), Vec3f(0, 0, 1));
    auto wall_right = std::make_shared<Rectangle>(Vec3f(1, 0, 1), Vec2f(2, 2), Vec3f(-1, 0, 0), Vec3f(0, 0, -1));
    auto wall_back = std::make_shared<Rectangle>(Vec3f(0, 1, 1), Vec2f(2, 2), Vec3f(0, -1, 0), Vec3f(1, 0, 0));

    // constant color material
    auto mat_white = std::make_shared<ConstColorMat>(Vec3f(0.7f, 0.7f, 0.7f));
    auto mat_red = std::make_shared<ConstColorMat>(Vec3f(0.8f, 0.f, 0.f));
    auto mat_green = std::make_shared<ConstColorMat>(Vec3f(0.f, 0.8f, 0.f));
    auto mat_yellow = std::make_shared<ConstColorMat>(Vec3f(1, 0.93, 0.6));
    auto mat_blue = std::make_shared<ConstColorMat>(Vec3f(0.2, 0.5, 0.9));

    // Textured material
    auto mat_with_norm = std::make_shared<TextureMat>();  // texture with normal
    mat_with_norm->setImageTexture("../textures/diff.jpg");
    mat_with_norm->setNormalTexture("../textures/norm.jpg");
    mat_with_norm->setShininess(16.0f);

    auto mat_without_norm = std::make_shared<TextureMat>();  // texture without normal
    mat_without_norm->setImageTexture("../textures/diff.jpg");
    mat_without_norm->setShininess(16.0f);

    auto mat_shanghaitech = std::make_shared<TextureMat>();  // ShanghaiTech texture
    mat_shanghaitech->setImageTexture("../textures/shanghaitech.jpg");
    mat_shanghaitech->setShininess(16.0f);
    
    switch (id) {
        case 0: {
            floor->setMaterial(mat_white);
            ceiling->setMaterial(mat_white);
            wall_back->setMaterial(mat_white);
            wall_left->setMaterial(mat_red);
            wall_right->setMaterial(mat_green);

            scene->addGeometry(floor);
            scene->addGeometry(ceiling);
            scene->addGeometry(wall_left);
            scene->addGeometry(wall_right);
            scene->addGeometry(wall_back);

            scene->addTetrahedron(mat_blue,
                                  Vec3f(-1, 0.2, 0.2),
                                  Vec3f(-0.5, -0.65, 0.2),
                                  Vec3f(0, 0.2, 0.2),
                                  Vec3f(-0.5, -0.3774, 1));

            scene->addEllipsoid(mat_yellow,
                                Vec3f(0.5, 0.4, 0.4),
                                Vec3f(0.3, 0.1, 0),
                                Vec3f(-0.1, 0.3, 0),
                                Vec3f(0, 0, 0.4));

            scene->setLight(square_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
            break;
        }

        case 1:
            // You can add your custom scene here.
            floor->setMaterial(mat_with_norm);
            ceiling->setMaterial(mat_white);
            wall_back->setMaterial(mat_with_norm);
            wall_left->setMaterial(mat_red);
            wall_right->setMaterial(mat_green);

            scene->addGeometry(floor);
            scene->addGeometry(ceiling);
            scene->addGeometry(wall_left);
            scene->addGeometry(wall_right);
            scene->addGeometry(wall_back);

            scene->addTetrahedron(mat_blue,
                                  Vec3f(-1, 0.2, 0.2),
                                  Vec3f(-0.5, -0.65, 0.2),
                                  Vec3f(0, 0.2, 0.2),
                                  Vec3f(-0.5, -0.3774, 1));

            scene->addEllipsoid(mat_yellow,
                                Vec3f(0.5, 0.4, 0.4),
                                Vec3f(0.3, 0.1, 0),
                                Vec3f(-0.1, 0.3, 0),
                                Vec3f(0, 0, 0.4));

            scene->setLight(square_light);
            scene->setAmbient(Vec3f(0.1, 0.1, 0.1));
            break;

        default:
            break;
    }
}
