#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// get 'model', 'view' and 'projection' matrix from CPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// tell fragmentShader where are the vertices
out vec3 Normal;  // tell fragment shader the normal vector
out vec3 FragPos;

out vec3 light_pos_view_1;
out vec3 light_pos_view_2;


void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
    Normal = vec3((transpose(inverse(view * model))) * vec4(aNormal, 0.0));
    FragPos = vec3(view * model * vec4(aPos, 1.0f));

    vec3 light_pos_world_1 = vec3(5.0f, 7.0f, 8.0f);
    vec3 light_pos_world_2 = vec3(-3.0f, 2.0f, -3.0f);

    light_pos_view_1 = vec3(view * vec4(light_pos_world_1, 1.0));
    light_pos_view_2 = vec3(view * vec4(light_pos_world_2, 1.0));
}