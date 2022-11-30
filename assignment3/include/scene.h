#ifndef CS171_HW3_INCLUDE_SCENE_H_
#define CS171_HW3_INCLUDE_SCENE_H_

#include <vector>

#include "camera.h"
#include "image.h"
#include "geometry.h"
#include "light.h"
#include "interaction.h"

class Scene {
   public:
    Scene() = default;

    void addGeometry(const std::shared_ptr<Geometry> &geometry);

    void setLight(const std::shared_ptr<Light> &new_light);
    [[nodiscard]] const std::shared_ptr<Light> &getLight() const;

    void setAmbient(Vec3f ambient);
    [[nodiscard]] const Vec3f &getAmbient() const;

    bool isShadowed(Ray &shadow_ray);
    bool intersect(Ray &ray, Interaction &interaction);

    void addTetrahedron(const std::shared_ptr<Material> &mat, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);
    void addEllipsoid(const std::shared_ptr<Material> &mat, const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c);

   private:
    std::vector<std::shared_ptr<Geometry>> geometries;
    std::shared_ptr<Light> light;
    Vec3f ambient_light;
};

#endif  // CS171_HW3_INCLUDE_SCENE_H_
