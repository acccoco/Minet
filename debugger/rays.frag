/* 渲染光源的参考物 */

#version 330 core
out vec4 FragColor;

uniform vec3 ray_color;

void main()
{
    FragColor = vec4(ray_color, 1.0);
}
