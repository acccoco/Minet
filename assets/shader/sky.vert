#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexVec;


void main() {
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // 让天空盒的深度始终最大
    gl_Position = pos.xyww;

    TexVec = aPos;
}
