
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;             /* 物体本身的颜色 */

uniform vec3 light_dir;         /* 点光源的方向 */
uniform vec3 light_color;       /* 光源的颜色 */
uniform vec3 ambient;           /* 环境光照 */


/* lambert 着色 */
void main()
{
    float cos_theta = max(0.f, dot(normalize(Normal), -normalize(light_dir)));
    vec3 diffuse_color = color * (light_color * cos_theta + ambient);
    FragColor = vec4(diffuse_color, 1.0);
}
