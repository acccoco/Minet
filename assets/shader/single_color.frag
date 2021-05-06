/* 固定颜色 */

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

void main()
{
    FragColor = vec4(255.f, 215.f, 0.f, 255.f)/255.f;
}