#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in MyBlock {
    vec4 end_pos;
} gs_in[];

void gen_line(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gs_in[index].end_pos;
    EmitVertex();
    EndPrimitive();
}

void main() {
    gen_line(0);
    gen_line(1);
    gen_line(2);
}
