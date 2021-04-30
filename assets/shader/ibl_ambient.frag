#version 330 core

struct Material {
    float alpha;
    float metalness;
    vec3 albedo;
    float ao;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

/* ------------------------------------------------------ */
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform PointLight light;
uniform Material material;
uniform vec3 eye_pos;
uniform vec3 ambient;
uniform samplerCube cubemap_env;
/* ------------------------------------------------------ */


const float PI = 3.14159265359;

/* 根据 fresnel 计算反射比例 */
vec3 fresnel_Schlick(vec3 H, vec3 V, vec3 F0) {
    float hdotv = max(0.0, dot(H, -V));
    return F0 + (1 - F0) * pow(1 - hdotv, 5);
}

/* 计算法线分布 */
float NDF_GGX(vec3 N, vec3 H, float alpha) {
    float alpha2 = alpha * alpha;
    float ndoth = max(0.0, dot(N, H));

    float nom = alpha2;
    float denom = PI * pow(ndoth * ndoth * (alpha2 - 1) + 1, 2);

    return nom / max(denom, 0.0000001);
}

/* 计算几何函数 */
float geometry_Schlick_GGX(vec3 N, vec3 V, float k) {
    float ndotv = max(0.0, dot(N, -V));
    float nom = ndotv;
    float denom = ndotv * (1 - k) + k;
    return nom / denom;
}

float geometry_Smith(vec3 N, vec3 V, vec3 L, float k) {
    return geometry_Schlick_GGX(N, V, k) * geometry_Schlick_GGX(N, L, k);
}


vec3 ambient_ibl(vec3 N, vec3 V, vec3 F0, Material m) {
    vec3 F = fresnel_Schlick(N, V, F0);
    vec3 k_diffuse = 1.0 - F;
    k_diffuse *= 1.0 - m.metalness;

    vec3 irradiance = texture(cubemap_env, N).xyz;
    vec3 ambient = k_diffuse * irradiance * m.albedo * m.ao;
    return ambient;
}


/* 整个反射函数，包括 diffuse 和 specular */
vec3 BRDF_Cook_Torrance(vec3 N, vec3 V, vec3 L, Material m, vec3 F0) {
    vec3 H = -normalize(V + L);
    float k = (m.alpha + 1) * (m.alpha + 1) / 8;

    float NDF = NDF_GGX(N, H, m.alpha);
    float G = geometry_Smith(N, V, L, k);
    vec3 F = fresnel_Schlick(H, V, F0);

    vec3 DFG = NDF * G * F;
    float denom = 4 * max(0.0, dot(-L, N)) * max(0.0, dot(-V, N));

    vec3 specular = DFG / max(0.00001, denom);
    
    vec3 k_diffuse = vec3(1.0) - F;
    k_diffuse *= 1.0 - m.metalness;

    return k_diffuse * (m.albedo / PI) + specular;
}


void main() {
    vec3 N = normalize(Normal);
    vec3 V = normalize(FragPos - eye_pos);

    vec3 Lo = vec3(0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metalness);

    // 遍历每个点光源，计算漫反射光照（Lambert 模型）和高光（Cook Torrance 模型）
    vec3 L = normalize(FragPos - light.position);
    float NdotL = max(0.0, dot(N, -L));

    // 随距离衰减
    float distance = length(light.position - eye_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 Li = light.color * attenuation;

    Lo += BRDF_Cook_Torrance(N, V, L, material, F0) * Li * NdotL;


    // 环境光
    vec3 ambient = ambient_ibl(N, V, F0, material);

    // 最终的颜色
    vec3 color = ambient + Lo;

    // HDR 映射
    color = color / (color + vec3(1.0));

    // Gamma 校正
    color = pow(color, vec3(1.0/2.2));

    gl_FragColor = vec4(color, 1.0);
}