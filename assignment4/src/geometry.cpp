#include "geometry.h"

#include <iostream>
#include <utility>

TriangleMesh::TriangleMesh(std::vector<Vec3f> vertices, std::vector<Vec3f> normals, std::vector<int> v_index,
                           std::vector<int> n_index)
    : vertices(std::move(vertices)),
      normals(std::move(normals)),
      v_indices(std::move(v_index)),
      n_indices(std::move(n_index)) {
}

bool TriangleMesh::intersect(Ray &ray, Interaction &interaction) const {
    // We have implemented global BVH.
    // Instead of building BVH for each object, we build a single BVH for the whole scene.
    // So we do not need to consider BVH intersection within this class. BVH codes are commented.

    // Loop through all triangles in the mesh and test intersection for each triangle.
    for (int i = 0; i < v_indices.size() / 3; i++) {
        Vec3i v_idx(v_indices[3 * i], v_indices[3 * i + 1], v_indices[3 * i + 2]);
        Vec3i n_idx(n_indices[3 * i], n_indices[3 * i + 1], n_indices[3 * i + 2]);
        Interaction temp;
        if (intersectOneTriangle(ray, temp, v_idx, n_idx) && (temp.dist < interaction.dist)) {
            interaction = temp;
        }
    }

    return interaction.type != Interaction::Type::NONE;
}

void TriangleMesh::setMaterial(std::shared_ptr<BSDF> &new_bsdf) {
    bsdf = new_bsdf;
}

bool TriangleMesh::intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i &v_idx,
                                        const Vec3i &n_idx) const {
    Vec3f v0 = vertices[v_idx[0]];
    Vec3f v1 = vertices[v_idx[1]];
    Vec3f v2 = vertices[v_idx[2]];
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);

    float invDet = 1.0f / det;

    Vec3f tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;
    Vec3f qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qvec) * invDet;
    if (t < ray.t_min || t > ray.t_max) return false;

    interaction.dist = t;
    interaction.pos = ray(t);
    interaction.normal = (u * normals[n_idx[1]] + v * normals[n_idx[2]] + (1 - u - v) * normals[n_idx[0]]).normalized();
    interaction.material = bsdf;
    interaction.type = Interaction::Type::GEOMETRY;
    return true;
}

// Excerpt From: https://developer.nvidia.com/blog/thinking-parallel-part-iii-tree-construction-gpu/
// Expands a 10-bit integer into 30 bits by inserting 2 zeros after each bit.
inline static unsigned int expandBits(unsigned int v) {
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}

// Excerpt From: https://developer.nvidia.com/blog/thinking-parallel-part-iii-tree-construction-gpu/
// Calculates a 30-bit Morton code for the given 3D point located within the unit cube [0,1].
inline static unsigned int morton3D(const Vec3f& point) {
    float x = point.x(), y = point.y(), z = point.z();
    x = std::min(std::max(x * 1024.0f, 0.0f), 1023.0f);
    y = std::min(std::max(y * 1024.0f, 0.0f), 1023.0f);
    z = std::min(std::max(z * 1024.0f, 0.0f), 1023.0f);
    unsigned int xx = expandBits((unsigned int)x);
    unsigned int yy = expandBits((unsigned int)y);
    unsigned int zz = expandBits((unsigned int)z);
    return xx * 4 + yy * 2 + zz;
}

unsigned int TriangleMesh::calcMortonCode(const Vec3f &pos, const AABB &box) {
    return morton3D((pos - box.low_bnd).array() / (box.upper_bnd - box.low_bnd).array());
}

// Generate an outmost AABB which contains all the triangles inside the triangle mesh.
AABB TriangleMesh::getAABB() const {
    AABB aabb;
    if (v_indices.size() / 3 > 0) {
        Vec3f v0 = vertices[v_indices[0]];
        Vec3f v1 = vertices[v_indices[1]];
        Vec3f v2 = vertices[v_indices[2]];
        aabb.merge(v0, v1, v2);
    }

    for (int i = 0; i < v_indices.size() / 3; i++) {
        const Vec3f v0 = vertices[v_indices[3 * i]];
        const Vec3f v1 = vertices[v_indices[3 * i + 1]];
        const Vec3f v2 = vertices[v_indices[3 * i + 2]];
        aabb.merge(v0, v1, v2);
    }
    return aabb;
}

void TriangleMesh::addToGlobalTriangles(std::vector<Triangle> &global_triangles, AABB &box) {
    for (int i = 0; i < v_indices.size() / 3; i++) {
        const Vec3f v0 = vertices[v_indices[3 * i]];
        const Vec3f v1 = vertices[v_indices[3 * i + 1]];
        const Vec3f v2 = vertices[v_indices[3 * i + 2]];
        
        const Vec3f n0 = normals[v_indices[3 * i]];
        const Vec3f n1 = normals[v_indices[3 * i + 1]];
        const Vec3f n2 = normals[v_indices[3 * i + 2]];

        global_triangles.emplace_back(
            v0, v1, v2, n0, n1, n2, bsdf, AABB(v0, v1, v2), calcMortonCode((v0 + v1 + v2) / 3, box)
        );
    }
}

bool Triangle::intersect(Ray &ray, Interaction &interaction) const {
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = ray.direction.cross(v0v2);

    float det = v0v1.dot(pvec);
    float invDet = 1.0f / det;

    Vec3f tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;
    Vec3f qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;
    float t = v0v2.dot(qvec) * invDet;
    if (t < ray.t_min || t > ray.t_max) return false;

    interaction.dist = t;
    interaction.pos = ray(t);
    interaction.normal = (u * n1 + v * n2 + (1 - u - v) * n0).normalized();
    interaction.type = Interaction::Type::GEOMETRY;
    interaction.material = bsdf;
    return true;
}
