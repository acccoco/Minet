#ifndef RENDER_ENGINE_SHADER_EXT_LIGHT_H
#define RENDER_ENGINE_SHADER_EXT_LIGHT_H

#include <string>

#include "./light.h"
#include "./shader.h"

/**
 * 适配各种 Light 类和 shader 的
 */
class ShaderExtLight {
private:

    /* 设置光的基本颜色 */
    static void set_light(Shader &shader, const Light &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".color.ambient", light.ambient);
        shader.uniform_vec3_set(name + ".color.diffuse", light.diffuse);
        shader.uniform_vec3_set(name + ".color.specular", light.specular);
    }

    /* 设置距离衰减系数 */
    static void set_attenuation(Shader &shader, const AttenuationDistance& attenuation, const std::string &name) {
        shader.uniform_float_set(name + ".constant", attenuation.constant);
        shader.uniform_float_set(name + ".linear", attenuation.linear);
        shader.uniform_float_set(name + ".quadratic", attenuation.quadratic);
    }

public:
    static void set(Shader &shader, const PointLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".position", light.position);

        set_attenuation(shader, light.attenuation, name);

        set_light(shader, light, name);
    }

    static void set(Shader &shader, const DirLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".direction", light.direction);

        set_light(shader, light, name);
    }

    static void set(Shader &shader, const SpotLight &light, const std::string &name) {
        shader.uniform_vec3_set(name + ".position", light.position);
        shader.uniform_vec3_set(name + ".direction", light.direction);

        set_light(shader, light, name);

        set_attenuation(shader, light.attenuation, name);

        // 设置切光角
        shader.uniform_float_set(name + ".inner_cutoff", light.inner_cutoff);
        shader.uniform_float_set(name + ".outer_cutoff", light.outer_cutoff);
    }

};

#endif //RENDER_SHADER_EXT_LIGHT_H
