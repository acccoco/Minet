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
public:
    void init() override {
        this->model_nano = ModelBuilder::build(ASSETS("nanosuit/nanosuit.obj"));
        this->tex_shader = std::make_shared<Shader>(SHADER("tex.vert"), SHADER("tex.frag"));
    }

    void update() override {
        this->tex_shader->uniform_mat4_set(EMatrix::view, Camera::view_matrix());
        this->tex_shader->uniform_mat4_set(EMatrix::projection, Camera::proj_matrix_get());

        this->model_nano->draw(this->tex_shader);
    }


};

#endif //RENDER_SCENE_NANO_H
