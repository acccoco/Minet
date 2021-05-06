#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 position;

uniform mat4 model;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main()
{
    vec4 pos_world = model * vec4(aPos, 1.0);
    gl_Position = projection * view * pos_world;

    position = vec3(pos_world);
    normal = transpose(inverse(mat3(model))) * aNormal;
}