/* 线性深度可视化 */

#version 330 core

struct Material {
    sampler2D texture_diffuse_0;// 物体在漫反射、环境光下的颜色
    sampler2D texture_specular_0;// 物体高光的颜色

    float shininess;// 反光度，影响高光光斑的大小
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform Material material;

void main()
{
    vec4 diffuse = texture(material.texture_diffuse_0, TexCoord);
    vec4 specular = texture(material.texture_specular_0, TexCoord);

    FragColor = diffuse + specular * 1.f;
}
