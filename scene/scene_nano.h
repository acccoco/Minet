#ifndef RENDER_SCENE_NANO_H
#define RENDER_SCENE_NANO_H

#include <memory>

#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"

#include "../config.hpp"

class SceneNano : public Scene {
private:
    std::shared_ptr<Model> model_nano;
    std::shared_ptr<Shader> tex_shader;

    std::shared_ptr<Camera> camera;

    void _init() override {
        this->camera = std::make_shared<Camera>();

        this->model_nano = ModelBuilder::build(ASSETS("nanosuit/nanosuit.obj"));
        this->tex_shader = std::make_shared<Shader>(SHADER("tex.vert"), SHADER("tex.frag"));
    }

    void _update() override {
        this->camera->update();

        this->tex_shader->uniform_mat4_set(ShaderMatrixName::view, this->camera->view_matrix_get());
        this->tex_shader->uniform_mat4_set(ShaderMatrixName::projection, this->camera->projection_matrix_get());

        this->model_nano->draw(this->tex_shader);
    }


};

#endif //RENDER_SCENE_NANO_H
