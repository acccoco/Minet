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
    float temp = 0;
    temp += texture(material.texture_specular_0, TexCoord).x + material.shininess;
    temp += texture(material.texture_diffuse_0, TexCoord).x;
    temp *= 0.00000000001;

    float non_linear_NDC = gl_FragCoord.z;
    float sum = 100.1;
    float diff = 99.9;
    float linear_NDC = (sum * non_linear_NDC - diff) / (sum - non_linear_NDC * diff);
    float z = linear_NDC/2 + 0.5;
    FragColor = vec4(vec3(z), 1.0) + temp;
}
