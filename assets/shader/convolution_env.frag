#version 330 core


in vec3 FragPos;
out vec4 FragColor;
uniform samplerCube cubemap_hdr;

const float PI = 3.14159265359;


void main() {
    const int n1 = 250;
    const int n2 = 250;

    vec3 N = normalize(FragPos);
    vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), N));
    vec3 front = normalize(cross(N, right));

    vec3 Lo = vec3(0.0);
    for (int m = 0; m < n1; ++m) {
        float theta = PI / 2 * m / n1;
        for (int n = 0; n < n2; ++n) {
            float phi = 2 * PI * n / n2;

            // 切线空间（右手系）：right 是x轴，front是y轴，N是z轴
            vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 I = tangent.x * right + tangent.y * front + tangent.z * N;
            vec3 Li = texture(cubemap_hdr, I).rgb;
            Lo += Li * cos(theta) * sin(theta);
        }
    }
    Lo = Lo * PI / n1 / n2;

    FragColor = vec4(Lo, 1.0);
}
