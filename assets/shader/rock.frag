/* 绘制一个岩石 */

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

out vec4 FragColor;


void main() {
    FragColor = texture(material.texture_diffuse_0, fs_in.TexCoord);
}
