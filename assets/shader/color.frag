/* 没有本色的物体 */

#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;


void main() {
    vec3 light_dir = normalize(vec3(-1.f, -1.f, 1.f));
    vec3 light_color = vec3(0.5f, 0.5f, 0.5f);
    vec3 env_color = vec3(0.2f, 0.2f, 0.2f);

    vec3 color = light_color * max(0.f, dot(-light_dir, normalize(Normal))) + env_color;
    gl_FragColor = vec4(color, 1.0f);
}
