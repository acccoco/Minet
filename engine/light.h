/**
 * 各种光源类型
 */
#ifndef RENDER_ENGINE_LIGHT_H
#define RENDER_ENGINE_LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"


/**
 * 光源的距离衰减系数
 */
struct AttenuationCoeffDistance {
    float constant;     // 常数项
    float linear;       // 一次项
    float quadratic;    // 二次项
};


/**
 * 光的颜色部分
 */
struct LightColor {
    glm::vec3 ambient;          // 环境颜色
    glm::vec3 diffuse;          // 漫反射颜色
    glm::vec3 specular;         // 高光颜色
};


/* 点光源：有颜色，有位置，会随着距离衰减 */
struct PointLight {
    LightColor color{};
    glm::vec3 position{};      // 光源的位置
    AttenuationCoeffDistance attenuation{};
};


/* 定向光：有颜色，有方向，不会衰减 */
struct DirLight {
    LightColor color{};
    glm::vec3 direction{};
};


/* 聚光：有颜色，有光源位置，有方向，会随着距离衰减，有限的照射角度 */
struct SpotLight {
    LightColor color{};
    glm::vec3 position{};
    glm::vec3 direction{};
    AttenuationCoeffDistance attenuation{};

    float inner_cutoff{};         // 内切光角
    float outer_cutoff{};         // 外切光角
};


/**
 * 适配各种 Light 类和 shader 的
 */
class ShaderExtLight {
public:
    /* 设置点光源的 uniform 变量 */
    static void set_point_light_uniform(Shader &shader, const PointLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".position", light.position);
        /* 距离衰减系数 */
        set_attenuation(shader, light.attenuation, name);
        /* 光的颜色 */
        set_light_color(shader, light.color, name);
    }

    /* 设置方向光的 uniform 变量 */
    static void set_dir_light_uniform(Shader &shader, const DirLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".direction", light.direction);
        /* 光的颜色 */
        set_light_color(shader, light.color, name);
    }

    /* 设置聚光的 uniform 变量 */
    static void set_spot_light_uniform(Shader &shader, const SpotLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".position", light.position);
        shader.uniform_vec3_set(name + ".direction", light.direction);
        /* 光的颜色 */
        set_light_color(shader, light.color, name);
        /* 距离衰减系数 */
        set_attenuation(shader, light.attenuation, name);
        // 设置切光角
        shader.uniform_float_set(name + ".inner_cutoff", light.inner_cutoff);
        shader.uniform_float_set(name + ".outer_cutoff", light.outer_cutoff);
    }

private:
    /* 设置光的基本颜色的 uniform 变量 */
    static void set_light_color(Shader &shader, const LightColor &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".color.ambient", light.ambient);
        shader.uniform_vec3_set(name + ".color.diffuse", light.diffuse);
        shader.uniform_vec3_set(name + ".color.specular", light.specular);
    }

    /* 设置距离衰减系数的 uniform 变量 */
    static void set_attenuation(Shader &shader, const AttenuationCoeffDistance &attenuation, const std::string &name) {
        shader.uniform_float_set(name + ".constant", attenuation.constant);
        shader.uniform_float_set(name + ".linear", attenuation.linear);
        shader.uniform_float_set(name + ".quadratic", attenuation.quadratic);
    }
};


#endif //RENDER_LIGHT_H
