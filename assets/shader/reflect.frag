#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 position;

uniform vec3 eye_pos;
uniform samplerCube sky_texture;

void main()
{
    vec3 view_dir = normalize(position - eye_pos);
    vec3 reflect_dir = reflect(view_dir, normalize(normal));
    FragColor = vec4(texture(sky_texture, reflect_dir).rgb, 1.0);
}
