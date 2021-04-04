#ifndef RENDER_SCENE2_H
#define RENDER_SCENE2_H

#include "../engine/scene.h"
#include "../engine/color.h"

#include "./light_box.h"


class Scene2 : public Scene {
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Model> model;

public:
    void init() override {
        this->shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));
        this->shader->uniform_vec3_set("light_color", Color::cyan1);

        this->model = std::make_shared<LightBox>();
        this->model->move(glm::vec3(0.f, 0.f, -5.f));
    }

    void update() override {
        this->shader->use();
        this->shader->uniform_mat4_set("view", Camera::view_matrix());
        this->shader->uniform_mat4_set("projection", Camera::proj_matrix_get());
        this->model->draw(this->shader);
        glUseProgram(0);
    }
};

#endif //RENDER_SCENE2_H
