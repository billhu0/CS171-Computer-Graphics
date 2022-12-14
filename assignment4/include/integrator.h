#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

#include "camera.h"
#include "interaction.h"
#include "scene.h"

class Integrator {
   public:
    Integrator(std::shared_ptr<Camera> cam, std::shared_ptr<Scene> scene, int spp, int max_depth);
    void render() const;
    Vec3f radiance(Ray &ray, Sampler &sampler, int depth) const;

   private:
    Vec3f directLighting(Interaction &interaction, Sampler &sampler) const;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Scene> scene;
    int max_depth;
    int spp;
    int spp_sqrt;
};

#endif  // INTEGRATOR_H_
