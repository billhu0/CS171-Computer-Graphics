#include "scene.h"

#include <utility>

void Scene::addGeometry(const std::shared_ptr<Geometry> &geometry) {
    geometries.push_back(geometry);
}

void Scene::setLight(const std::shared_ptr<Light> &new_light) {
    light = new_light;
}

bool Scene::isShadowed(Ray &shadow_ray) {
    // TODO: Your code here.
    Interaction interaction;

    if (intersect(shadow_ray, interaction)) {
        if (interaction.type == Interaction::GEOMETRY) {
            return true;
        }
    }

    return false;
}

bool Scene::intersect(Ray &ray, Interaction &interaction) {
    // TODO: Your code here.
    light->intersect(ray, interaction);

    // traverse all the geometries and check intersection
    for (auto & geometry: geometries) {
        Interaction tmp;
        if (geometry->intersect(ray, tmp)) {
            // Intersect found!
            // Check if the new interaction is of shorter distance, or no intersection.
            // If so, store it.
            if (interaction.type == Interaction::NONE || tmp.dist < interaction.dist) {
                interaction = tmp;
            }
        }
    }

    // check if the interaction is within the ray's t_min and t_max range
    if (interaction.type != Interaction::NONE && interaction.dist >= ray.t_min && interaction.dist <= ray.t_max) {
        return true;
    }

    // Otherwise, this ray is has no intersection with any geometries.
    return false;
}

const std::shared_ptr<Light> &Scene::getLight() const {
    return light;
}

void Scene::setAmbient(Vec3f ambient) {
    ambient_light = std::move(ambient);
}

const Vec3f &Scene::getAmbient() const {
    return ambient_light;
}

void Scene::addTetrahedron(const std::shared_ptr<Material> &mat, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, const Vec3f &v3) {
    std::shared_ptr<Geometry> f0 = std::make_shared<Triangle>(v0, v2, v1);
    std::shared_ptr<Geometry> f1 = std::make_shared<Triangle>(v0, v1, v3);
    std::shared_ptr<Geometry> f2 = std::make_shared<Triangle>(v1, v2, v3);
    std::shared_ptr<Geometry> f3 = std::make_shared<Triangle>(v0, v3, v2);
    f0->setMaterial(mat);
    f1->setMaterial(mat);
    f2->setMaterial(mat);
    f3->setMaterial(mat);
    addGeometry(f0);
    addGeometry(f1);
    addGeometry(f2);
    addGeometry(f3);
}

//void addTetrahedronToScene(std::shared_ptr<Scene> &scene, std::shared_ptr<Material> &mat,
//                           const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, const Vec3f &v3) {
//    std::shared_ptr<Geometry> f0 = std::make_shared<Triangle>(v0, v2, v1);
//    std::shared_ptr<Geometry> f1 = std::make_shared<Triangle>(v0, v1, v3);
//    std::shared_ptr<Geometry> f2 = std::make_shared<Triangle>(v1, v2, v3);
//    std::shared_ptr<Geometry> f3 = std::make_shared<Triangle>(v0, v3, v2);
//    f0->setMaterial(mat);
//    f1->setMaterial(mat);
//    f2->setMaterial(mat);
//    f3->setMaterial(mat);
//    scene->addGeometry(f0);
//    scene->addGeometry(f1);
//    scene->addGeometry(f2);
//    scene->addGeometry(f3);
//}

void Scene::addEllipsoid(const std::shared_ptr<Material> &mat, const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c) {
    std::shared_ptr<Geometry> ellipsoid = std::make_shared<Ellipsoid>(p, a, b, c);
    ellipsoid->setMaterial(mat);
    addGeometry(ellipsoid);
}

//void addEllipsoidToScene(std::shared_ptr<Scene> &scene, std::shared_ptr<Material> &mat,
//                         const Vec3f &p, const Vec3f &a, const Vec3f &b, const Vec3f &c) {
//    std::shared_ptr<Geometry> ellipsoid = std::make_shared<Ellipsoid>(p, a, b, c);
//    ellipsoid->setMaterial(mat);
//    scene->addGeometry(ellipsoid);
//}