
#include <memory>

#include "engine/render.h"
#include "engine/scene.h"
#include "engine/light.h"
#include "engine/color.h"

#include "assets/obj/cube.h"
#include "config.hpp"

std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/light/{}", EXAMPLE_DIR, file_name);
}


/* 多个光源的场景：点光源，聚光 */
class SceneLight : public Scene {
private:
    void _init() override {
        /* 为模型设置 texture */
        for (auto &mesh: box_meshes) {
            mesh->add_texture(TextureType::diffuse, tex_box_diffuse);
            mesh->add_texture(TextureType::specular, tex_box_specular);
        }

        /* 数据绑定 box-shader，每帧，场景 */
        box_shader->set_update_per_frame([this](Shader &shader){
            shader.uniform_vec3_set("eye_pos", Render::camera->position());
            ShaderExtLight::set_spot_light_uniform(shader, this->spot_light, "spot_light");
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });

        /* 数据绑定 light-shader：每帧，场景 */
        light_shader->set_update_per_frame([](Shader &shader){
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });

        /* 数据绑定：常量，box-shader */
        with(Shader, *box_shader) {
            /* 设置点光源的 uniform 属性 */
            ShaderExtLight::set_point_light_uniform(*box_shader, point_lights[0], "point_lights[0]");
            ShaderExtLight::set_point_light_uniform(*box_shader, point_lights[1], "point_lights[1]");
            ShaderExtLight::set_point_light_uniform(*box_shader, point_lights[2], "point_lights[2]");
            ShaderExtLight::set_point_light_uniform(*box_shader, point_lights[3], "point_lights[3]");

            /* 设置方向光的 uniform 属性 */
            ShaderExtLight::set_dir_light_uniform(*box_shader, dir_light, "dir_light");

            /* 设置聚光的 uniform 属性 */
            ShaderExtLight::set_spot_light_uniform(*box_shader, spot_light, "spot_light");

            /* 设置 box 的光滑程度 */
            this->box_shader->uniform_float_set("material.shininess", 128.f);
        }

        /* 数据绑定 box-shader：mesh */
        box_shader->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.set_textures(mesh, {
                    {"material.texture_diffuse_0",  TextureType::diffuse,  0},
                    {"material.texture_specular_0", TextureType::specular, 0},
            });
        });

        /* 数据绑定 light-shader：mesh */
        light_shader->set_drawT([](Shader &shader, const Mesh &mesh, const glm::vec3 &diffuse) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.uniform_vec3_set("light_color", diffuse);
        });
    }

    void _update() override {
        /* 更新场景信息 */
        spot_light.position = Render::camera->position();
        spot_light.direction = Render::camera->front();

        /* shader 同步场景信息 */
        box_shader->update_per_frame();
        light_shader->update_per_frame();

        /* 绘制表示光源的盒子 */
        withT (ShaderT<glm::vec3>, *light_shader) {
            for (unsigned i = 0; i < point_light_meshes.size(); ++i) {
                light_shader->draw_t(*point_light_meshes[i], point_lights[i].color.diffuse);
            }
        }

        /* 绘制场景中的箱子 */
        with(Shader, *box_shader) {
            for (auto &model : this->box_meshes) {
                box_shader->draw(*model);
            }
        }
    }


private:
    /* 光源随距离衰减的系数 */
    AttenuationCoeffDistance attenuation{1.f, 0.09f, 0.032f};

    /* 环境光的属性 */
    glm::vec3 ambient{0.05f, 0.05f, 0.05f};

    /* 所有的点光源 */
    std::vector<PointLight> point_lights{
            {{ambient, Color::aquamarine2,    Color::white}, glm::vec3(0.7f, 0.2f, 2.0f),    attenuation},
            {{ambient, Color::rosy_brown,     Color::white}, glm::vec3(2.3f, -3.3f, -4.0f),  attenuation},
            {{ambient, Color::indian_red1,    Color::white}, glm::vec3(-4.0f, 2.0f, -12.0f), attenuation},
            {{ambient, Color::deep_sky_blue2, Color::white}, glm::vec3(0.0f, 0.0f, -3.0f),   attenuation},
            {{ambient, Color::deep_sky_blue2, Color::white}, glm::vec3(0.0f, 0.0f, -7.0f),   attenuation},
    };

    /* 点光源对应的模型 */
    std::vector<std::shared_ptr<Mesh>> point_light_meshes{
            std::make_shared<Mesh>(cube_pnt_0_5, point_lights[0].position),
            std::make_shared<Mesh>(cube_pnt_0_5, point_lights[1].position),
            std::make_shared<Mesh>(cube_pnt_0_5, point_lights[2].position),
            std::make_shared<Mesh>(cube_pnt_0_5, point_lights[3].position),
            std::make_shared<Mesh>(cube_pnt_0_5, point_lights[4].position),
    };

    /* 方向光 */
    DirLight dir_light{{ambient, Color::gray41, Color::white}, glm::vec3(-0.2f, -1.0f, -0.3f)};

    /* 聚光 */
    SpotLight spot_light{{Color::white, Color::black, Color::white},
                         Render::camera->position(), Render::camera->front(), attenuation,
                         glm::cos(glm::radians(12.5f)),
                         glm::cos(glm::radians(17.5f))};

    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    std::shared_ptr<Texture2D> tex_box_specular = std::make_shared<Texture2D>(TEXTURE("container2_specular.jpg"));

    /* 场景中的模型 */
    std::vector<std::shared_ptr<Mesh>> box_meshes{
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(0.0f, 0.0f, 0.0f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(2.0f, 5.0f, -15.0f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(-1.5f, -2.2f, -2.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(-3.8f, -2.0f, -12.3f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(2.4f, -0.4f, -3.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(-1.7f, 3.0f, -7.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(1.3f, -2.0f, -2.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(1.5f, 2.0f, -2.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(1.5f, 0.2f, -1.5f)),
            std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(-1.3f, 1.0f, -1.5f))
    };

    /* 用于渲染 box 的着色器 */
    std::shared_ptr<Shader> box_shader = std::make_shared<Shader>(CUR_DIR("phong.vert"), CUR_DIR("phong.frag"));

    /* 用于渲染点光源的 shader */
    std::shared_ptr<ShaderT<glm::vec3>> light_shader = std::make_shared<ShaderT<glm::vec3>>(CUR_DIR("light.vert"),
                                                                                            CUR_DIR("light.frag"));
};


int main() {
    Render::init();
    Render::render<SceneLight>();
    Render::terminate();
    return 0;
}
