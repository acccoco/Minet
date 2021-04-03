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

class Light {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


/**
 * 点光源
 */
class PointLight: public Light {
public:
    glm::vec3 position;      // 光源的位置
    AttenuationDistance attenuation{};

    PointLight() noexcept {}
};


/**
 * 定向光
 */
class DirLight: public Light {
public:
    glm::vec3 direction;
};


/**
 * 聚光
 */
class SpotLight: public Light {
public:
    glm::vec3 position;
    glm::vec3 direction;

    float inner_cutoff;     // 内切光角
    float outer_cutoff;      // 外切光角

    AttenuationDistance attenuation;
};




#endif //RENDER_LIGHT_H
