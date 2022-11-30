#pragma once

#include "defines.h"
#include <vector>

struct Vertex {
    vec3 position;
    vec3 normal;
};

class Mesh {
public:
    /* construct a mesh from a obj file */
    Mesh(const std::string &path);

    /* draw mesh objects */
    void draw() const;

private:
    // mesh data
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    // VAO, VBO, EBO ids
    GLuint VAO = 0;  // Vertex Array Object
    GLuint VBO = 0;  // Vertex Buffer Object
    GLuint EBO = 0;  // Element Buffer Object / Index Buffer Object

    void loadDataFromFile(const std::string &path);
};
