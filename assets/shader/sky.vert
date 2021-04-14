#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out vec3 TexVec;


void main() {
    vec3 p = mat3(view) * aPos;
    vec4 pos = projection * vec4(p, 1.0);
    // 让天空盒的深度始终最大
    gl_Position = pos.xyww;

    TexVec = aPos;
}
