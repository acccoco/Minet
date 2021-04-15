#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceModel;

out Block {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

layout (std140)  uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform float time;


mat4 rotate_x(float angle) {
    mat4 res;
    res[0] = vec4(1, 0, 0, 0);
    res[1] = vec4(0, cos(angle), sin(angle), 0);
    res[2] = vec4(0, -sin(angle), cos(angle), 0);
    res[3] = vec4(0, 0, 0, 1);
    return res;
}

void main() {

    mat4 rotate_model = instanceModel * rotate_x(0.04 * time);
    gl_Position = projection * view * rotate_model * vec4(aPos, 1.0);

    vs_out.FragPos = vec3(rotate_model * vec4(aPos, 1.0));
    vs_out.TexCoord = aTexCoord;
    vs_out.Normal = mat3(transpose(inverse(rotate_model))) * aNormal;
}
