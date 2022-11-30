#version 330 core
layout (triangles) in;  // We are using GL_TRIANGLES as input primitive type in vertexShader
layout (line_strip, max_vertices = 6) out;

uniform mat4 proj;

in VS_OUT {
    vec3 normal;
} gs_in[];  // receive from vertex shader

const float MAGNITUDE = 0.005;

void GenerateLine(int index) {
    gl_Position = proj * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = proj * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}