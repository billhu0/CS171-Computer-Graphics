#include "scene.h"
#include "load_obj.h"

#include <iostream>

void Scene::addObject(std::shared_ptr<TriangleMesh> &mesh) {
    objects.push_back(mesh);
}

void Scene::setLight(const std::shared_ptr<Light> &new_light) {
    light = new_light;
}

bool Scene::isShadowed(Ray &shadow_ray) {
    Interaction in;
    return intersect(shadow_ray, in) && in.type == Interaction::Type::GEOMETRY;
}

bool Scene::intersect(Ray &ray, Interaction &interaction) {

    #ifdef USE_GLOBAL_BVH
    /* With-BVH acceleration structure implementation.*/
    // Check intersection with BVH, not with each object.
    if (bvh_root != nullptr) {
        light->intersect(ray, interaction);
        #ifdef USE_LINEARIZED_BVH
        return LinearBVHHit(ray, interaction);
        #else 
        return bvhHit(ray, interaction, bvh_root);
        #endif
    }

    // We must have a BVH here! If no BVH, there must be an error!
    assert(false);
    return false;
    
    #else 

    /* Ordinary implementation (without BVH acceleration). */
    // Traverse each object (TriangleMesh) in the scene.
    light->intersect(ray, interaction);
    for (const auto &obj : objects) {
        Interaction cur_it;
        if (obj->intersect(ray, cur_it) && (cur_it.dist < interaction.dist)) {
            interaction = cur_it;
        }
    }
    return interaction.type != Interaction::Type::NONE;

    #endif

}

const std::shared_ptr<Light> &Scene::getLight() const {
    return light;
}

void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene) {
    // add square light to scene.
    std::shared_ptr<Light> light = std::make_shared<SquareAreaLight>(
        Vec3f(config.light_config.position), Vec3f(config.light_config.radiance), Vec2f(config.light_config.size));
    scene->setLight(light);
    // init all materials.
    std::map<std::string, std::shared_ptr<BSDF>> mat_list;
    for (const auto &mat : config.materials) {
        std::shared_ptr<BSDF> p_mat;
        switch (mat.type) {
            case MaterialType::DIFFUSE: {
                p_mat = std::make_shared<IdealDiffusion>(Vec3f(mat.color));
                mat_list[mat.name] = p_mat;
                break;
            }
            case MaterialType::SPECULAR: {
                p_mat = std::make_shared<IdealSpecular>(Vec3f(mat.color));
                mat_list[mat.name] = p_mat;
                break;
            }
            default: {
                std::cerr << "unsupported material type!" << std::endl;
                exit(-1);
            }
        }
    }
    // add mesh objects to scene. Translation and scaling are directly applied to
    // vertex coordinates. then set corresponding material by name.
    std::cout << "loading obj files..." << std::endl;
    for (auto &object : config.objects) {
        auto mesh_obj = makeMeshObject(object.obj_file_path, Vec3f(object.translate), object.scale);
        mesh_obj->setMaterial(mat_list[object.material_name]);
        // We don't build a BVH for each object. Instead, we build a global BVH.
        // Therefore, the following sentence is commented.
        // if (object.has_bvh) {
        //     mesh_obj->buildBVH();
        // }
        scene->addObject(mesh_obj);
    }

    #ifdef USE_GLOBAL_BVH
    // build a global BVH for the whole scene
    scene->build_global_BVH();
    #endif
}

#ifdef USE_GLOBAL_BVH

// Scene BVH construction

void Scene::build_global_BVH() {
    // Calculate the AABB large enough to hold the whole scene
    AABB scene_box;
    if (!objects.empty()) {
        scene_box = objects[0]->getAABB();
    }
    for (const std::shared_ptr<TriangleMesh> &object: objects) {
        scene_box.merge(object->getAABB());
    }

    // Calculate morton code for each triangle.
    // For each triangleMesh, calculate their morton code and merge them to a large triangle vector.
    triangles.clear();
    for (const std::shared_ptr<TriangleMesh> &object: objects) {
        // std::vector<Triangle> tmp_triangles = object->getTrianglesAndMortonCode(scene_box);

        // Merge all triangles into a large vector of triangles
        // triangles.insert(triangles.end(), tmp_triangles.begin(), tmp_triangles.end());
        object->addToGlobalTriangles(triangles, scene_box);
    }

    // Sort triangles by their morton code.
    // Lambda function is used as a comparator: [] (Triangle a, b) { return a.mortonCode > b.mortonCode }
    std::sort(triangles.begin(), triangles.end(), [](const Triangle &a, const Triangle &b) { return a.morton_code > b.morton_code; });

    // Construct BVH
    bvh_root = generateHierarchy(0, (int) triangles.size() - 1);

    #ifdef USE_LINEARIZED_BVH
    // Construct linearized BVH
    genLinearBVH(bvh_root);
    #endif
}


// Excerpt from: https://developer.nvidia.com/blog/thinking-parallel-part-iii-tree-construction-gpu/
// Top-Down Hierarchy Generation
BVHNode *Scene::generateHierarchy(const int first, const int last) {

    // Single object (or less than 8 object) --> Create a leaf node
    int len = last - first + 1;
    if (len >= 1 && len <= 8) {
        return newLeafNode(first, last);
    }

    // Determine where to split the range
    int split = findSplit(first, last);

    // Process the resulting sub-ranges recursively
    BVHNode *childA = generateHierarchy(first, split);
    BVHNode *childB = generateHierarchy(split + 1, last);
    return newInternalNode(childA, childB);

}

// Excerpt from: https://developer.nvidia.com/blog/thinking-parallel-part-iii-tree-construction-gpu/
int Scene::findSplit(int first, int last) {
    // Get morton code
    unsigned int firstCode = triangles[first].morton_code;
    unsigned int lastCode = triangles[last].morton_code;

    // Identical morton codes --> split range in the middle
    if (firstCode == lastCode) {
        return (first + last) >> 1;
    }

    // Calculate the number of the highest bits that are the same for all objects,
    // using the count-leading-zeros intrinsic.
    int commonPrefix = __builtin_clz(firstCode ^ lastCode);

    // Use binary search to find where the next bit differs.
    // We want to find the highest object that shares more than commonPrefix bits with the first one.
    int split = first;
    int step = last - first;
    do {
        step = (step + 1) >> 1;  // exponential decrease
        int newSplit = split + step;  // proposed new position

        if (newSplit < last) {
            unsigned int splitCode = triangles[newSplit].morton_code;
            int splitPrefix = __builtin_clz(firstCode ^ splitCode);
            if (splitPrefix > commonPrefix) {
                split = newSplit;  // accept proposal
            }
        }
    } while (step > 1);

    return split;
}

// Create a new leaf BVH node containing triangles [start:end]
BVHNode *Scene::newLeafNode(int start, int end) {
    BVHNode *node = new BVHNode();
    node->start = start;
    node->end = end;

    AABB aabb = triangles[start].aabb;
    for (int i = start; i <= end; i++) {
        aabb.merge(triangles[i].aabb);
    }
    node->aabb = aabb;

    node->left = nullptr;
    node->right = nullptr;
    node->size = 1;
    return node;
}

// Create a new Internal BVH node given its child
BVHNode *Scene::newInternalNode(BVHNode *left, BVHNode *right) {
    BVHNode *node = new BVHNode();
    node->aabb = AABB(left->aabb, right->aabb);
    node->left = left;
    node->right = right;
    node->size = left->size + right->size + 1;
    return node;
}

#ifndef USE_LINEARIZED_BVH

// Check intersection with the ray. Recursive function.
bool Scene::bvhHit(Ray &ray, Interaction &interaction, BVHNode *node) {
    float t_in, t_out;

    // If the node is null, return false
    if (!node) {
        return false;
    }

    // If the ray does not intersect with the AABB of the node, return false
    if (!node->aabb.intersect(ray, &t_in, &t_out)) {
        return false;
    }

    // If the node is a leaf node (both left and right child are null),
    // check intersection with each triangle inside it
    if (!node->left && !node->right) {
        for (int i = node->start; i <= node->end; i++) {
            Interaction in;
            if (triangles[i].intersect(ray, in) && in.dist < interaction.dist) {
                interaction = in;
            }
        }
        return interaction.type != Interaction::NONE;
    }

    // Otherwise, if the node is an internal node, use recursion:
    // check intersection with its left child and right child respectively
    return bvhHit(ray, interaction, node->left) || bvhHit(ray, interaction, node->right);
}

#endif

// Generate linearized BVH by recursion.
void Scene::genLinearBVH(BVHNode *node) {
    if (!node) {
        return;
    }

    LinearBVHNode linear_node;
    linear_node.aabb = node->aabb;

    // If this node is a leaf node (both left and right are nullptr)
    if (!node->left && !node->right) {
        linear_node.start = node->start;
        linear_node.end = node->end;
    } else {
        linear_node.right = (int) linear_bvh_nodes.size() + node->left->size + 1;
    }
    linear_bvh_nodes.push_back(linear_node);

    // recursively generate linear BVH for its left and right child
    genLinearBVH(node->left);
    genLinearBVH(node->right);
}

// Linear BVH hit is same in theory as the ordinary BVH hit function.
// The difference is that we have to 'rewrite' a 'leftChild', 'rightChild' function.
bool Scene::LinearBVHHit(Ray &ray, Interaction &interaction) {
    // DFS traversal
    std::stack<int> fringe;  // the nodes we need to visit
    int curr_node = 0;  // store the index of the current visiting node
    bool hit = false;

    while (true) {
        // Start from curr_node (index), check intersection with AABB
        LinearBVHNode node = linear_bvh_nodes[curr_node];
        float t_in, t_out;
        if (!node.aabb.intersect(ray, &t_in, &t_out)) {
            if (fringe.empty()) {
                break;
            }
            curr_node = fringe.top();
            fringe.pop();
        }

        // If the node is a leaf node, check intersection with the triangles inside it.
        if (node.start != -1) {
            for (int i = node.start; i <= node.end; i++) {
                Interaction in;
                if (triangles[i].intersect(ray, in) && in.dist < interaction.dist) {
                    interaction = in;
                }
            }
            if (interaction.type != Interaction::Type::NONE) {
                hit = true;
            }
            if (fringe.empty()) {
                break;
            }
            curr_node = fringe.top();
            fringe.pop();
        }

        // If the current node is an internal node.
        // We need to check if its left and right child
        else {
            // check intersection with right child
            float t_in_right, t_out_right;
            bool hit_right = linear_bvh_nodes[node.right].aabb.intersect(ray, &t_in_right, &t_out_right);

            // check intersection with left child
            float t_in_left, t_out_left;
            bool hit_left = linear_bvh_nodes[curr_node + 1].aabb.intersect(ray, &t_in_left, &t_out_left);

            // If it can hit both left and right child, choose the closer one
            if (hit_left && hit_right) {
                if (t_in_left < t_in_right) {
                    fringe.push(node.right);
                    curr_node++;
                } else {
                    fringe.push(curr_node + 1);
                    curr_node = node.right;
                }
            }

            // if only left child is hit
            else if (hit_left) {
                curr_node++;
            }

            // if only right child is hit
            else if (hit_right) {
                curr_node = node.right;
            }

            // if both are not hit
            else {
                if (fringe.empty()) {
                    break;
                }
                curr_node = fringe.top();
                fringe.pop();
            }
        }
    }
    return hit;
}

#endif // USE_GLOBAL_BVH
