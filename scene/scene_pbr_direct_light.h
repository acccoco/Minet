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


char buffer[256];

class ScenePbrDirectLight : public Scene {
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


public:
    void _init() override {
        camera = std::make_shared<Camera>();

        mesh_sphere = std::make_shared<Sphere>();
        mesh_box = std::make_shared<PNTMesh>(box_mesh);

        pbr_shader = std::make_shared<Shader>(SHADER("pbr_direct_light.vert"), SHADER("pbr_direct_light.frag"));
        light_shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));
    }

    void _update() override {
        camera->update();

        // 绘制光源的参考物
        with(Shader, *light_shader) {
            light_shader->uniform_mat4_set("view", camera->view_matrix_get());
            light_shader->uniform_mat4_set("projection", camera->projection_matrix_get());
            light_shader->uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), light.position));
            light_shader->uniform_vec3_set("light_color", light.color);
            with(PNTMesh, *mesh_box) {
                glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
            }
        }

        // 绘制 pbr 球体
        with(Shader, *pbr_shader) {
            pbr_shader->uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 0.f, -4.f)));
            pbr_shader->uniform_mat4_set("view", camera->view_matrix_get());
            pbr_shader->uniform_mat4_set("projection", camera->projection_matrix_get());
            pbr_shader->uniform_vec3_set("eye_pos", camera->position_get());

            // 光源的属性
            pbr_shader->uniform_vec3_set("light.position", light.position);
            pbr_shader->uniform_vec3_set("light.color", light.color);
            pbr_shader->uniform_vec3_set("ambient", ambient);

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
        ImGui::ColorEdit3("albedo", (float *) &material.albedo);
        ImGui::End();

        ImGui::Begin("light");
        ImGui::DragFloat3("position", (float *) &light.position);
        ImGui::ColorEdit3("color", (float *) &light.color);
        ImGui::ColorEdit3("ambient", (float *) &ambient);
        ImGui::End();
    }

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<Sphere> mesh_sphere;
    std::shared_ptr<Shader> pbr_shader;
    std::shared_ptr<Shader> light_shader;

    glm::vec3 ambient{0.04f};

    Material material{0.1, 0.9, glm::vec3(0.5, 0.0, 0.0), 1.0};
    PLight light{glm::vec3(-4.0f, 1.0f, 4.0f), glm::vec3(300.0f, 300.0f, 300.0f)};
};

#endif //RENDER_SCENE_PBR_DIRECT_LIGHT_H
