/* 等距柱状投影的 hdr 贴图变成立方体贴图 */

#version 330 core

in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_hdr;

const float PI = 3.14159265359;

void main() {
    // 根据坐标计算出 uv
    vec3 pos = normalize(FragPos);
    float theta = asin(pos.y);
    float phi = atan(pos.z, pos.x);
    vec2 uv = vec2(phi / (2 * PI) + 0.5f, theta / PI + 0.5f);

    // 从贴图中采样
    vec3 color = texture(texture_hdr, uv).rgb;
    FragColor = vec4(color, 1.0);
}
