#include "load_obj.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include <tiny_obj_loader.h>

static bool loadObj(const std::string &path, std::vector<Vec3f> &vertices, std::vector<Vec3f> &normals,
                    std::vector<int> &v_index, std::vector<int> &n_index) {
    std::cout << "-- Loading model " << path << std::endl;

    tinyobj::ObjReaderConfig readerConfig;
    // readerConfig.mtl_search_path = "./";  // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, readerConfig)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertices.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
    }

    for (size_t i = 0; i < attrib.normals.size(); i += 3) {
        normals.emplace_back(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]);
    }

    // Loop over shapes
    for (const tinyobj::shape_t &shape : shapes) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            auto fv = size_t(shape.mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                v_index.push_back(idx.vertex_index);
                n_index.push_back(idx.normal_index);
            }
            index_offset += fv;
        }
    }
    std::cout << "  # vertices: " << attrib.vertices.size() / 3 << std::endl;
    std::cout << "  # faces: " << v_index.size() / 3 << std::endl;
    return true;
}

std::shared_ptr<TriangleMesh> makeMeshObject(const std::string& path_to_obj, const Vec3f& translation, float scale) {
    std::vector<Vec3f> vertices;
    std::vector<Vec3f> normals;
    std::vector<int> v_idx;
    std::vector<int> n_idx;
    loadObj(path_to_obj, vertices, normals, v_idx, n_idx);
    for (auto &v : vertices) v = v * scale + translation;
    return std::make_shared<TriangleMesh>(vertices, normals, v_idx, n_idx);
}