#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "accel.h"
#include "bsdf.h"
#include "core.h"
#include "interaction.h"
#include "ray.h"

#include <optional>
#include <vector>

class Triangle;

class TriangleMesh {
   public:
    TriangleMesh() = default;
    TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals, std::vector<int> v_index, std::vector<int> n_index);
    bool intersect(Ray &ray, Interaction &interaction) const;
    void setMaterial(std::shared_ptr<BSDF> &new_bsdf);

    // Generate an outmost AABB which contains all the triangles inside the triangle mesh.
    [[nodiscard]] AABB getAABB() const;

    // Given a scene AABB box, return all triangles along with their morton code
    void addToGlobalTriangles(std::vector<Triangle> &global_triangles, AABB &box);

    // calculate morton code given a triangle's gravity center 
    static unsigned int calcMortonCode(const Vec3f& pos, const AABB& box);

   private:
    bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i &v_idx, const Vec3i &n_idx) const;
    std::shared_ptr<BSDF> bsdf;

    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<int> v_indices;
    std::vector<int> n_indices;
};


// A triangle class. Contains a single triangle and morton code
class Triangle {
   public:
    Triangle(Vec3f v0, Vec3f v1, Vec3f v2, Vec3f n0, Vec3f n1, Vec3f n2, std::shared_ptr<BSDF> material, AABB aabb, unsigned int code):
        v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)), n0(std::move(n0)), n1(std::move(n1)), n2(std::move(n2)), bsdf(std::move(material)), aabb(aabb), morton_code(code) {}
    bool intersect(Ray &ray, Interaction &interaction) const;

   public:
    Vec3f v0, v1, v2, n0, n1, n2;
    AABB aabb;
    std::shared_ptr<BSDF> bsdf;

   public:
    unsigned int morton_code;
};

#endif  // GEOMETRY_H_