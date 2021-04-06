#ifndef RENDER_MYSCENE_H
#define RENDER_MYSCENE_H

#include <memory>

#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"

#include "../config.hpp"
#include "./happy_box.h"
#include "./light_box.h"



class MyScene : public Scene {
private:
    // 光源随距离衰减的系数
    AttenuationDistance attenuation = AttenuationDistance{1.f, 0.09f, 0.032f};

    // 环境光的属性
    glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);

    // 场景中的光源
    std::shared_ptr<DirLight> dir_light;
    std::shared_ptr<SpotLight> spot_light;
    std::vector<std::shared_ptr<PointLight>> point_lights;

    // 用到的着色器
    std::shared_ptr<Shader> box_shader;
    std::shared_ptr<Shader> light_shader;

    // 要渲染的模型
    std::vector<std::shared_ptr<Model>> point_light_models;
    std::vector<std::shared_ptr<Model>> box_models;

    // 摄像机
    std::shared_ptr<Camera> camera;

    void _init() override {
        // 初始化相机
        this->camera = std::make_shared<Camera>();

        SPDLOG_INFO("_init all light in scene");
        this->spot_light_init();
        this->dir_light_init();
        this->point_light_init();

        SPDLOG_INFO("create light box in scene");
        this->point_light_init();

        SPDLOG_INFO("create happy box in scene");
        this->happy_box_init();

        SPDLOG_INFO("_init shader in scene");
        this->shader_init();
    }

    void _update() override {

        this->camera->update();

        light_box_draw();

        happy_box_draw();
    }

    void light_box_draw() {
        this->light_shader->uniform_mat4_set("view", this->camera->view_matrix_get());
        this->light_shader->uniform_mat4_set("projection", this->camera->projection_matrix_get());

        // 绘制光源 box
        for (unsigned int i = 0; i < 5; ++i) {
            this->light_shader->uniform_vec3_set("light_color", this->point_lights[i]->diffuse);
            this->point_light_models[i]->draw(this->light_shader);
        }
    }

    void happy_box_draw() {
        // 绘制 box
        this->box_shader->uniform_vec3_set("eye_pos", this->camera->position_get());
        this->spot_light->position = this->camera->position_get();
        this->spot_light->direction = this->camera->front_dir_get();
        ShaderExtLight::set(*this->box_shader, *this->spot_light, "spot_light");
        this->box_shader->uniform_mat4_set(ShaderMatrixName::view, this->camera->view_matrix_get());
        this->box_shader->uniform_mat4_set(ShaderMatrixName::projection, this->camera->projection_matrix_get());
        for (auto &model : this->box_models) {
            model->draw(this->box_shader);
        }
    }

    /* 初始化点光源，以及参照盒子 */
    void point_light_init() {
        // 点光源的位置
        glm::vec3 light_positions[] = {
                glm::vec3(0.7f, 0.2f, 2.0f),
                glm::vec3(2.3f, -3.3f, -4.0f),
                glm::vec3(-4.0f, 2.0f, -12.0f),
                glm::vec3(0.0f, 0.0f, -3.0f),
                glm::vec3(0.0f, 0.0f, -7.0f),
        };

        // 点光源的颜色
        glm::vec3 light_colors[] = {
                Color::aquamarine2,
                Color::rosy_brown,
                Color::indian_red1,
                Color::deep_sky_blue2,
                Color::deep_sky_blue2,
        };

        for (int i = 0; i < 5; ++i) {
            // 设置光源属性
            auto light = std::make_shared<PointLight>(ambient, light_colors[i], Color::white);
            light->position = light_positions[i];
            light->attenuation = attenuation;
            this->point_lights.push_back(light);

            // 放置模型
            auto model = std::make_shared<LightBox>();
            model->move(light_positions[i]);
            this->point_light_models.push_back(model);
        }
    }

    void spot_light_init() {
        // 聚光
        this->spot_light = std::make_shared<SpotLight>(Color::white, Color::black, Color::white);
        spot_light->direction = this->camera->front_dir_get();
        spot_light->attenuation = attenuation;
        spot_light->position = this->camera->position_get();
        spot_light->inner_cutoff = glm::cos(glm::radians(12.5f));
        spot_light->outer_cutoff = glm::cos(glm::radians(17.5f));
    }

    void dir_light_init() {
        // 定向光
        this->dir_light = std::make_shared<DirLight>(ambient, Color::gray41, Color::white);
        dir_light->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    }

    void shader_init() {
        // 着色器 1
        this->box_shader = std::make_shared<Shader>(SHADER("phong.vert"), SHADER("phong.frag"));
        ShaderExtLight::set(*this->box_shader, *dir_light, "dir_light");
        ShaderExtLight::set(*this->box_shader, *spot_light, "spot_light");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[0], "point_lights[0]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[1], "point_lights[1]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[2], "point_lights[2]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[3], "point_lights[3]");
        this->box_shader->uniform_float_set("material.shininess", 128.f);

        // 着色器 2
        this->light_shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));
    }

    void happy_box_init() {
        // 模型的位置
        std::vector<glm::vec3> model_positions = {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)
        };
        for (auto &model_position : model_positions) {
            auto happy_box = std::make_shared<HappyBox>();
            happy_box->move(model_position);
            this->box_models.push_back(happy_box);
        }
    }

};

#endif //RENDER_MYSCENE_H
