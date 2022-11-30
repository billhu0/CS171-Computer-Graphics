#ifndef OBJECT_H
#define OBJECT_H
#include "defines.h"
#include <shader.h>
#include <vector>

struct Vertex {
    vec3 position;
    vec3 normal;
};

class Object {
   private:
    unsigned int VAO{};
    unsigned int VBO{};
    unsigned int EBO{};

   public:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    Object();
    ~Object();

    void init();

    void drawElements() const;
};
#endif