#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include <memory>

#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/shader_ext_light.h"
#include "../engine/color_const.h"
#include "../engine/texture.h"

#include "./happy_box.h"
#include "./light_box.h"


class MyScene : public Scene {
protected:
    std::shared_ptr<DirLight> dir_light;
    std::shared_ptr<SpotLight> spot_light;
    std::shared_ptr<PointLight> point_lights[4];

    std::shared_ptr<Shader> box_shader;

public:
    void init() override {


        // 光源 =================================================================
        // 统一的距离衰减系数
        auto attenuation = AttenuationDistance{1.f, 0.09f, 0.032f};
        auto ambient = glm::vec3(0.05f, 0.05f, 0.05f);

        // 定向光
        this->dir_light = std::make_shared<DirLight>();
        dir_light->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        dir_light->ambient = ambient;
        dir_light->diffuse = Color::gray41;
        dir_light->specular = Color::white;

        // 聚光
        this->spot_light = std::make_shared<SpotLight>();
        spot_light->direction = Camera::get_front();
        spot_light->attenuation = attenuation;
        spot_light->position = Camera::get_eye();
        spot_light->inner_cutoff = glm::cos(glm::radians(12.5f));
        spot_light->outer_cutoff = glm::cos(glm::radians(17.5f));
        spot_light->diffuse = Color::white;
        spot_light->ambient = Color::black;
        spot_light->specular = Color::white;

        // 点光源的位置
        std::vector<glm::vec3> light_positions = {
                glm::vec3(0.7f, 0.2f, 2.0f),
                glm::vec3(2.3f, -3.3f, -4.0f),
                glm::vec3(-4.0f, 2.0f, -12.0f),
                glm::vec3(0.0f, 0.0f, -3.0f)
        };

        // 点光源的颜色
        std::vector<glm::vec3> light_colors = {
                Color::aquamarine2,
                Color::rosy_brown,
                Color::indian_red1,
                Color::deep_sky_blue2,
        };

        for (int i = 0; i < 4; ++i) {
            this->point_lights[i] = std::make_shared<PointLight>();
            this->point_lights[i]->position = light_positions[i];
            this->point_lights[i]->attenuation = attenuation;
            this->point_lights[i]->ambient = ambient;
            this->point_lights[i]->diffuse = light_colors[i];
            this->point_lights[i]->specular = Color::white;
        }


        // 贴图 =================================================================
        // 贴图: 铁箱子
        Texture2D tex_container2_diffuse(TEXTURE("container2.jpg"));
        Texture2D tex_container2_specular(TEXTURE("container2_specular.jpg"));

        // 贴图: 滑稽脸
        Texture2D tex_awesomeface_diffuse(TEXTURE("awesomeface.jpg"));


        // 模型: 铁箱子 ===========================================================
        // 着色器
        this->box_shader = std::make_shared<Shader>(SHADER("phong.vert"), SHADER("phong.frag"));
        ShaderExtLight::set(*this->box_shader, *dir_light, "dir_light");
        ShaderExtLight::set(*this->box_shader, *spot_light, "spot_light");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[0], "point_lights[0]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[1], "point_lights[1]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[2], "point_lights[2]");
        ShaderExtLight::set(*this->box_shader, *this->point_lights[3], "point_lights[3]");
        this->box_shader->uniform_tex2d_set("material.diffuse", tex_container2_diffuse.id);
        this->box_shader->uniform_tex2d_set("material.specular", tex_container2_specular.id);
        this->box_shader->uniform_float_set("material.shininess", 128.f);

        // 模型
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
            happy_box->bind_shader(this->box_shader);
            happy_box->translate(model_position);
            this->regist(happy_box);
        }


        // 模型: 光源盒子 ===========================================================
        for (auto &point_light : this->point_lights) {
            // 着色器
            auto light_shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));
            light_shader->uniform_vec3_set("light_color", point_light->diffuse);

            // 模型
            auto light_box = std::make_shared<LightBox>();
            light_box->bind_shader(light_shader);
            light_box->translate(point_light->position);
            this->regist(light_box);
        }

    }

    void update() override {
        this->box_shader->uniform_vec3_set("eye_pos", Camera::get_eye());

        this->spot_light->position = Camera::get_eye();
        this->spot_light->direction = Camera::get_front();
        ShaderExtLight::set(*this->box_shader, *this->spot_light, "spot_light");
    }
};

#endif //RENDER_SCENE_H
