#ifndef RENDER_ENGINE_SHADER_EXT_MVP_H
#define RENDER_ENGINE_SHADER_EXT_MVP_H

#include <glm/glm.hpp>

class ShaderExtMVP {
public:
    static void set(Shader &shader, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj) {
        shader.uniform_mat4_set("model", model);
        shader.uniform_mat4_set("view", view);
        shader.uniform_mat4_set("projection", proj);
    }
};

#endif //RENDER_SHADER_EXT_MVP_H
