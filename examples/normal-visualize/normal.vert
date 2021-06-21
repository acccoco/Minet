#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out MyBlock {
    vec4 end_pos;       /* 法线的端点（world 坐标系） */
} vs_out;

uniform mat4 model;

#ifdef UNIFORM_BLOCK
layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};
#else
uniform mat4 view;
uniform mat4 projection;
#endif


void main()
{
    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(aPos, 1.0);

    /* 计算出 world 坐标系中法线的端点 */
    vec3 end_in_model = aPos + aNormal * 0.4;
    vs_out.end_pos = mvp * vec4(end_in_model, 1.0);
}
