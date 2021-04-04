/**
 * 各种光源类型
 */
#ifndef RENDER_ENGINE_LIGHT_H
#define RENDER_ENGINE_LIGHT_H

#include <glm/glm.hpp>


/**
 * 距离衰减系数
 */
struct AttenuationDistance {
    float constant;
    float linear;
    float quadratic;
};


/**
 * 光线的基本定义
 */
class Light {
public:
    glm::vec3 ambient;          // 环境颜色
    glm::vec3 diffuse;          // 漫反射颜色
    glm::vec3 specular;         // 高光颜色

    Light(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular);
};


// 点光源：有颜色，有位置，会随着距离衰减
class PointLight : public Light {
public:
    glm::vec3 position;      // 光源的位置
    AttenuationDistance attenuation{};

    PointLight(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular)
            : Light(ambient, diffuse, specular) {}

};


/**
 * 定向光：有颜色，有方向
 */
class DirLight : public Light {
public:
    glm::vec3 direction;

    DirLight(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular)
            : Light(ambient, diffuse, specular) {}
};


/**
 * 聚光：有颜色，有光源位置，有方向，会随着距离衰减，有限的照射角度
 */
class SpotLight : public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;

    float inner_cutoff;         // 内切光角
    float outer_cutoff;         // 外切光角

    AttenuationDistance attenuation;

    SpotLight(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular)
            : Light(ambient, diffuse, specular) {}
};


#endif //RENDER_LIGHT_H
