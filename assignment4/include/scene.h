#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "camera.h"
#include "config.h"
#include "geometry.h"
#include "image.h"
#include "interaction.h"
#include "light.h"

class Scene {
   public:
    Scene() = default;
    void addObject(std::shared_ptr<TriangleMesh> &geometry);
    [[nodiscard]] const std::shared_ptr<Light> &getLight() const;
    void setLight(const std::shared_ptr<Light> &new_light);
    bool isShadowed(Ray &shadow_ray);
    bool intersect(Ray &ray, Interaction &interaction);

    // We want a global BVH for the whole scene, instead of constructing
    // an independent BVH for each object (or, class TriangleMesh).
    // Therefore, instead of storing the BVH data and functions in TriangleMesh,
    // we store it in the Scene class.

    // Build BVH (for the whole scene, not for each object)
    void build_global_BVH();

   private:
    std::vector<std::shared_ptr<TriangleMesh>> objects;
    std::shared_ptr<Light> light;

    // In the scene, we don't store a list of TriangleMesh, but we directly store Triangles.
    std::vector<Triangle> triangles;

    // BVH tree data 
    BVHNode *bvh_root = nullptr;

    // Generate Top-down BVH hierarchy
    BVHNode *generateHierarchy(int first, int last);

    // Assist function for 'generateHierarchy'
    int findSplit(int first, int last);

    // Create a new Leaf node or Internal Node
    BVHNode *newLeafNode(int start, int end);
    static BVHNode *newInternalNode(BVHNode *left, BVHNode *right);

    #ifndef USE_LINEARIZED_BVH
    // BVH hit
    bool bvhHit(Ray &ray, Interaction &interaction, BVHNode *node);
    #endif

    // Linear BVH data
    std::vector<LinearBVHNode> linear_bvh_nodes;

    // Linear BVH construction
    void genLinearBVH(BVHNode *node);

    // Linear BVH hit
    bool LinearBVHHit(Ray &ray, Interaction &interaction);
};

void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene);

#endif  // SCENE_H_
