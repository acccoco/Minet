#version 330 core

// 类型定义 =======================================================================
struct LightColor {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

/* 点光源 */
struct PointLight {
    vec3 position;

// 点光源随距离衰减的系数
    float constant;
    float linear;
    float quadratic;

    LightColor color;
};


/* 定向光 */
struct DirLight {
    vec3 direction;

    LightColor color;
};


/* 聚光 */
struct SpotLight {
    vec3 position;
    vec3 direction;

    LightColor color;

    // 角度衰减，cos
    float inner_cutoff;
    float outer_cutoff;

    // 距离衰减
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;// 物体在漫反射、环境光下的颜色
    sampler2D specular;// 物体高光的颜色

    float shininess;// 反光度，影响高光光斑的大小
};


// 全局变量 =======================================================================

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 eye_pos;// 观察者的位置
uniform Material material;

#define CNT_LIGHT_POINT 4
uniform PointLight point_lights[CNT_LIGHT_POINT];
uniform DirLight dir_light;
uniform SpotLight spot_light;


// 函数定义 =======================================================================
/**
 * 计算 phong 光照
 * @param light_dir 光线方向，单位向量
 * @param normal 片段的法线方向，单位向量
 * @param view_dir 视线方向，单位向量
 */
vec3 phong_calc(LightColor light_color, vec3 light_dir, vec3 normal, vec3 view_dir);

/**
 * 计算定向光的光照
 * @param normal 片段的法线，单位向量
 * @param view_dir 视线，单位向量
 */
vec3 dir_light_calc(DirLight light, vec3 normal, vec3 view_dir);

/**
 * 计算点光源的光照
 * @param normal 片段的法线，单位向量
 * @param view_dir 视线，单位向量
 */
vec3 point_light_calc(PointLight light, vec3 normal, vec3 view_dir);

/**
 * 计算聚光的光照
 * @param normal 片段的法线，单位向量
 * @param view_dir 视线，单位向量
 */
vec3 spot_light_calc(SpotLight light, vec3 normal, vec3 view_dir);


// ============================================================================
void main()
{
    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(FragPos - eye_pos);

    vec3 result = vec3(0, 0, 0);

    // 计算定向光照
    result += dir_light_calc(dir_light, norm, view_dir);

    // 计算所有的点光
    for (int i = 0; i < CNT_LIGHT_POINT; ++i)
        result += point_light_calc(point_lights[i], norm, view_dir);

    // 计算聚光
    result += spot_light_calc(spot_light, norm, view_dir);

    // 最终颜色
    FragColor = vec4(result, 1.0);
}


// 函数实现 =======================================================================
vec3 phong_calc(LightColor light_color, vec3 light_dir, vec3 normal, vec3 view_dir) {
    vec3 reflect_dir = reflect(light_dir, normal);

    // 漫反射，高光系数：和空间有关的
    float diff_coef = max(0.0, -dot(normal, light_dir));
    float spec_coef = pow(max(0.0, -dot(view_dir, reflect_dir)), material.shininess);

    // 环境、漫反射、高光颜色
    vec3 ambient = light_color.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light_color.diffuse * diff_coef * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light_color.specular * spec_coef * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular);
}


vec3 dir_light_calc(DirLight light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction);

    vec3 phong = phong_calc(light.color, light_dir, normal, view_dir);

    return phong;
}


vec3 point_light_calc(PointLight light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(FragPos - light.position);
    float distance = length(light.position - FragPos);

    vec3 phong = phong_calc(light.color, light_dir, normal, view_dir);

    // 随距离衰减的系数
    float attenuation_coef = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    return phong * attenuation_coef;
}


vec3 spot_light_calc(SpotLight light, vec3 normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction);
    float distance = length(light.position - FragPos);
    float theta = dot(light_dir, normalize(FragPos - light.position));

    vec3 phong = phong_calc(light.color, light_dir, normal, view_dir);

    float attenuation_dis = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    float attenuation_angle = clamp((theta - light.outer_cutoff) / (light.inner_cutoff - light.outer_cutoff), 0.0, 1.0);

    return phong * attenuation_dis * attenuation_angle;
}
