#ifndef RENDER_SCENE_BOX_FLOOR_H
#define RENDER_SCENE_BOX_FLOOR_H

#include <cmath>
#include <random>
#include <memory>
#include <pthread.h>
#include "../engine/utils/with.h"
#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/utils/with.h"
#include "../config.hpp"
#include "./obj/box.h"
#include "./obj/floor.h"


class SceneBoxFloor : public Scene {
public:

    void _init() override {
        // camera
        camera = std::make_shared<Camera>();

        // mesh
        mesh_box = std::make_shared<PNTMesh>(box_mesh);
        mesh_floor = std::make_shared<PNTMesh>(floor_mesh);

        // shader
        shader_blinn = std::make_shared<Shader>(SHADER("blinn_phong.vert"), SHADER("blinn_phong.frag"));
        shader_light = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));

        // texture
        tex_box = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
        tex_floor = std::make_shared<Texture2D>(TEXTURE("wood_floor.jpg"));
    }

    void _gui() override {
        ImGui::Begin("point light");

        ImGui::ColorEdit3("light color", (float*)&light_color);
        ImGui::Checkbox("blinn-phong", &blinn_phong);

        ImGui::DragFloat3("light pos", (float*)&light_pos, 0.01f);

        ImGui::End();
    }

    void _update() override {
        camera->update();

        // 绘制代表光源的盒子
        with(Shader, *shader_light) {
            shader_light->uniform_mat4_set("view", camera->view_matrix_get());
            shader_light->uniform_mat4_set("projection", camera->projection_matrix_get());

            shader_light->uniform_vec3_set("light_color", light_color);

            with(PNTMesh, *mesh_box) {
                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), light_pos);
                shader_light->uniform_mat4_set("model", model);
                glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
            }
        }

        with (Shader, *shader_blinn) {
            shader_blinn->uniform_vec3_set("eye_pos", camera->position_get());
            shader_blinn->uniform_mat4_set("view", camera->view_matrix_get());
            shader_blinn->uniform_mat4_set("projection", camera->projection_matrix_get());

            // 光照模型
            shader_blinn->uniform_int_set("blinn_phong", blinn_phong);

            // 光源颜色
            shader_blinn->uniform_vec3_set("plight0.pos", light_pos);
            shader_blinn->uniform_vec3_set("plight0.color", light_color);

            // 绘制箱子
            with(PNTMesh, *mesh_box) {
                glBindTexture(GL_TEXTURE_2D, tex_box->id);
                shader_blinn->uniform_tex2d_set("texture_diffuse_0", 0);

                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 1.f, 0.f));
                shader_blinn->uniform_mat4_set("model", model);

                glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
            }

            // 绘制地面
            with(PNTMesh, *mesh_floor) {
                glBindTexture(GL_TEXTURE_2D, tex_floor->id);
                shader_blinn->uniform_tex2d_set("texture_diffuse_0", 0);

                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, -1.f, 0.f));
                shader_blinn->uniform_mat4_set("model", model);

                glDrawArrays(GL_TRIANGLES, 0, mesh_floor->vertex_cnt);
            }
        }
    }

private:
    // 模型
    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<PNTMesh> mesh_floor;

    // 摄像机
    std::shared_ptr<Camera> camera;

    // 着色器
    std::shared_ptr<Shader> shader_blinn;
    std::shared_ptr<Shader> shader_light;

    // 纹理
    std::shared_ptr<Texture2D> tex_box;
    std::shared_ptr<Texture2D> tex_floor;

    // gui 参数
    bool blinn_phong = false;
    glm::vec3 light_color = Color::aquamarine2;
    glm::vec3 light_pos{2.f, 1.f, 2.f};
};

#endif //RENDER_SCENE_BOX_FLOOR_H
