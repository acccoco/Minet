#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include <memory>

#include <glm/glm.hpp>

#include "./model.h"
#include "./light.h"
#include "./shader_ext_light.h"
#include "./camera.h"
#include "./shader_ext_mvp.h"


class Scene {
protected:
    std::vector<std::shared_ptr<Model2>> models;

public:
    void regist(const std::shared_ptr<Model2> &model) {
        this->models.push_back(model);
    }

    /**
     * 初始化场景, 可以用于初始化光源,摄像机等
     */
    virtual void init() = 0;

    virtual void update() = 0;

    void draw() {
        glm::mat4 view = Camera::view_matrix();
        glm::mat4 projection = Camera::projection_matrix();

        for (const auto &model : this->models) {
            model->update();

            // 更新 MVP 矩阵
            ShaderExtMVP::set(*model->get_shader(), model->get_model_matrix(), view, projection);

            model->draw();
        }
    }
};


#endif //RENDER_SCENE_H
