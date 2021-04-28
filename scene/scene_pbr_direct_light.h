//
// Created by bigso on 2021/4/28.
//

#ifndef RENDER_SCENE_PBR_DIRECT_LIGHT_H
#define RENDER_SCENE_PBR_DIRECT_LIGHT_H

#include <memory>
#include <string>
#include "../engine/scene.h"
#include "../engine/mesh.h"
#include "./obj/sphere.h"
#include "./obj/box.h"

struct Material {
    float alpha;
    float metalness;
    glm::vec3 albedo;
    float ao;
};

struct PLight {
    glm::vec3 position;
    glm::vec3 color;
};

char buffer[256];

class ScenePbrDirectLight : public Scene {
public:
    void _init() override {
        camera = std::make_shared<Camera>();

        mesh_sphere = std::make_shared<Sphere>();
        mesh_box = std::make_shared<PNTMesh>(box_mesh);

        pbr_shader = std::make_shared<Shader>(SHADER("pbr_direct_light.vert"), SHADER("pbr_direct_light.frag"));
        light_shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));

        albedo = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_basecolor.png"));
        alpha = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_roughness.png"));
        metalness = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_metallic.png"));

        with (Shader, *pbr_shader) {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, albedo->id);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, alpha->id);
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, metalness->id);

            pbr_shader->uniform_tex2d_set("texture_albedo", 0);
            pbr_shader->uniform_tex2d_set("texture_alpha", 1);
            pbr_shader->uniform_tex2d_set("texture_metalness", 2);
        }


        // 初始化光源
        glm::vec3 light_pos[] = {
                glm::vec3(-10.0f, 10.0f, 10.0f),
                glm::vec3(10.0f, 10.0f, 10.0f),
                glm::vec3(-10.0f, -10.0f, 10.0f),
                glm::vec3(10.0f, -10.0f, 10.0f),
        };
        glm::vec3 light_color[] = {
                glm::vec3(300.0f, 300.0f, 300.0f),
                glm::vec3(300.0f, 300.0f, 300.0f),
                glm::vec3(300.0f, 300.0f, 300.0f),
                glm::vec3(300.0f, 300.0f, 300.0f)
        };
        for (int i = 0; i < 4; ++i) {
            lights[i].position = light_pos[i];
            lights[i].color = light_color[i];
        }
    }

    void _update() override {
        camera->update();

        with(Shader, *light_shader) {
            light_shader->uniform_mat4_set("view", camera->view_matrix_get());
            light_shader->uniform_mat4_set("projection", camera->projection_matrix_get());

            for (auto &light : lights) {
                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), light.position);
                light_shader->uniform_mat4_set("model", model);
                light_shader->uniform_vec3_set("light_color", light.color);
                with(PNTMesh, *mesh_box) {
                    glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
                }
            }
        }

        with(Shader, *pbr_shader) {
            pbr_shader->uniform_mat4_set("model", glm::one<glm::mat4>());
            pbr_shader->uniform_mat4_set("view", camera->view_matrix_get());
            pbr_shader->uniform_mat4_set("projection", camera->projection_matrix_get());
            pbr_shader->uniform_vec3_set("eye_pos", camera->position_get());

            // 光源的属性
            for (int i = 0; i < 4; ++i) {
                sprintf(buffer, "lights[%d].", i);
                pbr_shader->uniform_vec3_set(buffer + std::string("position"), lights[i].position);
                pbr_shader->uniform_vec3_set(buffer + std::string("color"), lights[i].color);
            }

            int nr_rows = 7;
            int nr_cols = 7;
            float spacing = 2.5f;
            for (int row = 0; row < nr_rows; ++row) {
                for (int col = 0; col < nr_cols; ++col) {
                    glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(
                            (float) (col - (nr_cols / 2)) * spacing,
                            (float) (row - (nr_rows / 2)) * spacing,
                            0.0f));
                    pbr_shader->uniform_mat4_set("model", model);
                    mesh_sphere->draw();
                }
            }
        }
    }

    void _gui() override {
        ImGui::Begin("material");

        ImGui::End();
    }

private:
    std::shared_ptr<Camera> camera;

    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<Sphere> mesh_sphere;

    std::shared_ptr<Shader> pbr_shader;
    std::shared_ptr<Shader> light_shader;

    std::shared_ptr<Texture2D> albedo;
    std::shared_ptr<Texture2D> alpha;
    std::shared_ptr<Texture2D> metalness;

    PLight lights[4];
};

#endif //RENDER_SCENE_PBR_DIRECT_LIGHT_H
