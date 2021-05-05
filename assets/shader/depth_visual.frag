/* 深度可视化：将非线性深度转换为线性深度，使用 灰色 显示出来 */
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    float non_linear_NDC = gl_FragCoord.z;
    float sum = 100.1;
    float diff = 99.9;
    float linear_NDC = (sum * non_linear_NDC - diff) / (sum - non_linear_NDC * diff);
    float z = linear_NDC/2 + 0.5;
    FragColor = vec4(vec3(z), 1.0);
}
