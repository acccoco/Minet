//
// Created by bigso on 2021/4/28.
//

#ifndef RENDER_SCENE_PBR_DIRECT_LIGHT_H
#define RENDER_SCENE_PBR_DIRECT_LIGHT_H

#include <memory>
#include "../engine/scene.h"
#include "./obj/sphere.h"

class ScenePbrDirectLight : public Scene{
public:
    void _init() override {
        camera = std::make_shared<Camera>();

        sphere = std::make_shared<Sphere>();

        shader = std::make_shared<Shader>(SHADER("color.vert"), SHADER("color.frag"));
        with(Shader, *shader) {
            shader->uniform_mat4_set("projection", camera->projection_matrix_get());
        }
    }

    void _update() override {
        camera->update();

        with(Shader, *shader) {
            shader->uniform_mat4_set("view", camera->view_matrix_get());
            shader->uniform_mat4_set("model", glm::one<glm::mat4>());
            sphere->draw();
        }
    }

    void _gui() override {

    }

private:
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Sphere> sphere;

    std::shared_ptr<Shader> shader;
};

#endif //RENDER_SCENE_PBR_DIRECT_LIGHT_H
