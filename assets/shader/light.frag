/* 渲染光源的参考物 */

#version 330 core
out vec4 FragColor;

uniform vec3 light_color;

void main()
{
    FragColor = vec4(light_color.x * 0.0001, 0.9, 0.9, 1.0);
}
