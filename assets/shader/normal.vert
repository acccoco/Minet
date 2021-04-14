#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out MyBlock {
    vec4 end_pos;
} vs_out;

uniform mat4 model;
layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};


void main()
{
    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(aPos, 1.0);
    vec3 end_in_model = aPos + aNormal * 0.4;
    vs_out.end_pos = mvp * vec4(end_in_model, 1.0);
}
