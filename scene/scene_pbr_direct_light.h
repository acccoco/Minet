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

        material = {0.1, 0.9, glm::vec3(0.5, 0.0, 0.0), 1.0};

        pbr_shader = std::make_shared<Shader>(SHADER("pbr_direct_light.vert"), SHADER("pbr_direct_light.frag"));
        light_shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));

        with(Shader, *pbr_shader) {
            pbr_shader->uniform_mat4_set("projection", camera->projection_matrix_get());
        }
        with(Shader, *light_shader) {
            light_shader->uniform_mat4_set("projection", camera->projection_matrix_get());
        }

        // 初始化光源
        glm::vec3 light_pos[] = {
                glm::vec3(-4.0f, 1.0f, 4.0f),
                glm::vec3(4.0f, 1.0f, 4.0f),
                glm::vec3(-4.0f, -1.0f, 4.0f),
                glm::vec3(4.0f, -1.0f, 4.0f),
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

            for (auto & light : lights) {
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
            pbr_shader->uniform_vec3_set("eye_pos", camera->position_get());

            // 光源的属性
            for (int i = 0; i < 4; ++i) {
                sprintf(buffer, "lights[%d].", i);
                pbr_shader->uniform_vec3_set(buffer + std::string("position"), lights[i].position);
                pbr_shader->uniform_vec3_set(buffer + std::string("color"), lights[i].color);
            }

            // 物体的参数
            pbr_shader->uniform_float_set("material.alpha", material.alpha);
            pbr_shader->uniform_float_set("material.metalness", material.metalness);
            pbr_shader->uniform_vec3_set("material.albedo", material.albedo);
            pbr_shader->uniform_float_set("material.ao", material.ao);

            mesh_sphere->draw();
        }
    }

    void _gui() override {
        ImGui::Begin("material");

        ImGui::SliderFloat("alpha", &material.alpha, 0, 1);
        ImGui::SliderFloat("metalness", &material.metalness, 0, 1);
        ImGui::ColorEdit3("albedo", (float*)&material.albedo);

        ImGui::End();
    }

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<Sphere> mesh_sphere;
    std::shared_ptr<Shader> pbr_shader;
    std::shared_ptr<Shader> light_shader;

    Material material;
    PLight lights[4];
};

#endif //RENDER_SCENE_PBR_DIRECT_LIGHT_H
