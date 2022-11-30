#include "mesh.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <vector>

Mesh::Mesh(const std::string &path) { loadDataFromFile(getPath(path)); }

/**
 * Load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /**
     * path stands for the where the object is stored
     * */
    // open file
    std::ifstream infile(path);
    if (!infile) {
        std::cerr << "Failed to open file \'" << path << "\'\n";
        return;
    }

    // read file
    std::vector<glm::vec3> positions, normals;
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream lineStream(line);

        // temp variables
        char t;  // first char in a line. will be thrown away.
        float x, y, z;

        if (line.empty() || line[0] == '#') continue;  // blank line or comment line
        if (line[0] == 'v') {  // position of a vertex
            lineStream >> t >> x >> y >> z;
            Vertex v{};
            v.position = glm::vec3(x, y, z);
            m_vertices.push_back(v);
        } else if (line[0] == 'n') {  // normal of a vertex
            lineStream >> t >> x >> y >> z;
            normals.emplace_back(x, y, z);
        } else if (line[0] == 'f') {  // a triangle face with vertex index and normal index
            Vertex a0{}, a1{}, a2{};
            GLuint v0, v1, v2, n0, n1, n2;
            lineStream >> t >> v0 >> n0 >> v1 >> n1 >> v2 >> n2;
            m_indices.push_back(v0);
            m_indices.push_back(v1);
            m_indices.push_back(v2);
            m_vertices[v0].normal = normals[n0];
            m_vertices[v1].normal = normals[n1];
            m_vertices[v2].normal = normals[n2];
        }
    }

    // close the file
    infile.close();

    // move the positions and normals to Mesh object itself
    size_t num_vertices = positions.size();
    for (auto i = 0; i < num_vertices; i++) {
        m_vertices.push_back({positions[i], normals[i]});
    }

    // init the objects

    // Generate VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate VBO
    glGenBuffers(1, &VBO);  // Generate a VBO (vertex buffer object)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Bind the generated VBO to GL_ARRAY_BUFFER (vertex buffer)
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);  // Copy data to bound buffer

    // Generate EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

    // Vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}