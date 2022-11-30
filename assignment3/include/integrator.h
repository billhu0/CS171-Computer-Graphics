#ifndef CS171_HW3_INCLUDE_INTEGRATOR_H_
#define CS171_HW3_INCLUDE_INTEGRATOR_H_

#include "camera.h"
#include "scene.h"
#include "interaction.h"

class PhongLightingIntegrator {
   public:
    PhongLightingIntegrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene);
    ~PhongLightingIntegrator() = default;
    void render() const;
    Vec3f radiance(Ray &ray, Interaction &interaction) const;

   private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
};

#endif  // CS171_HW3_INCLUDE_INTEGRATOR_H_
