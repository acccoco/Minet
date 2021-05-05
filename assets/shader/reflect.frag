#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 position;

uniform vec3 eye_pos;
uniform float reflect_indensity;
uniform samplerCube sky_texture;

void main()
{
    // 计算反射的光强
    vec3 view_dir = normalize(position - eye_pos);
    vec3 reflect_dir = reflect(view_dir, normalize(normal));
    vec3 reflect_color = texture(sky_texture, reflect_dir).rgb;

    // 折射的颜色
    float refract_ratio = 1.00 / 1.52;
    vec3 refract_dir = refract(view_dir, normalize(normal), refract_ratio);
    vec3 refract_color = texture(sky_texture, refract_dir).rgb;

    // 反射强度和折射强度
    float refract_indensity = 1.0 - reflect_indensity;
    FragColor = vec4(reflect_indensity * reflect_color + refract_indensity * refract_color, 1.0);
}
