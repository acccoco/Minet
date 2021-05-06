/* 批量绘制 instance */

#version 330 core

in Block {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

struct Material {
    sampler2D texture_diffuse_0;
};

uniform Material material;


void main() {
    gl_FragColor = texture(material.texture_diffuse_0, fs_in.TexCoord);
}
