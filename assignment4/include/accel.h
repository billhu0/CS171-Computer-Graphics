#ifndef ACCEL_H_
#define ACCEL_H_

#include <utility>

#include "core.h"
#include "ray.h"

struct AABB {
    // the minimum and maximum coordinate for the AABB
    Vec3f low_bnd;
    Vec3f upper_bnd;

    AABB() : low_bnd(0, 0, 0), upper_bnd(0, 0, 0) {}
    AABB(Vec3f low, Vec3f upper) : low_bnd(std::move(low)), upper_bnd(std::move(upper)) {}

    // construct an AABB from three vertices of a triangle.
    AABB(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);

    // Construct AABB by merging two AABBs
    AABB(const AABB &a, const AABB &b);

    // Merge AABB
    void merge(const AABB& other);
    void merge(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);

    // test intersection with given ray.
    // ray distance of entrance and exit point are recorded in t_in and t_out
    bool intersect(const Ray &ray, float *t_in, float *t_out);

    // Get the AABB center
    [[nodiscard]] Vec3f getCenter() const { return (low_bnd + upper_bnd) / 2; }

    // Get the length of a specified side on the AABB
    [[nodiscard]] float getDist(int dim) const { return upper_bnd[dim] - low_bnd[dim]; }

    // Check whether the AABB is overlapping with another AABB
    [[nodiscard]] bool isOverlap(const AABB &other) const;
};

struct BVHNode {
    BVHNode *left = nullptr;
    BVHNode *right = nullptr;
    
    // bounding box of current node.
    AABB aabb;

    // Triangle indices (start and end) and size of current BVH leaf node.
    // Note: Triangles are sorted by morton code before constructing BVH, so instead of storing
    // an std::vector<int> triangles, we can store the start and end index of the triangles.
    int start = -1, end = -1, size = 0;
};

// You may need to add your code for BVH construction here.

struct LinearBVHNode {
    AABB aabb;
    union {
        int start = -1;
        int left;
    };
    union {
        int end = -1;
        int right;
    };
};

#endif  // ACCEL_H_