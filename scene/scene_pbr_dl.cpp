

#include <memory>
#include <string>
#include "../engine/scene.h"
#include "../engine/mesh.h"
#include "./obj/sphere.h"
#include "./obj/box.h"

#include "../engine/core.h"

char buffer[256];


/* PBR：直接光照 */
class ScenePbrDL : public Scene {
    /* 球体的物理参数 */
    struct Material {
        float alpha;
        float metalness;
        glm::vec3 albedo;
        float ao;
    };

    /* 点光源的属性 */
    struct PLight {
        glm::vec3 position;
        glm::vec3 color;
    };


public:
    void _init() override {
        camera = std::make_shared<Camera>();

        mesh_sphere = std::make_shared<Sphere>();
        mesh_box = std::make_shared<PNTMesh>(box_mesh);

        shader_pbr_texture = std::make_shared<Shader>(SHADER("pbr_direct_light.vert"),
                                                      SHADER("pbr_direct_light.frag"),
                                                      std::vector<std::string>{"MATERIAL_TEXTURE"});
        shader_pbr_material = std::make_shared<Shader>(SHADER("pbr_direct_light.vert"),
                                                       SHADER("pbr_direct_light.frag"));
        shader_light = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));

        tex_albedo = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_basecolor.png"));
        tex_metalness = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_metallic.png"));
        tex_roughness = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_roughness.png"));
    }

    void _update() override {
        camera->update();

        // 绘制光源的参考物
        with(Shader, *shader_light) {
            shader_light->uniform_mat4_set("view", camera->view_matrix_get());
            shader_light->uniform_mat4_set("projection", camera->projection_matrix_get());
            shader_light->uniform_mat4_set("model",
                                           glm::translate(glm::one<glm::mat4>(), light.position));
            shader_light->uniform_vec3_set("light_color", light.color);
            with(PNTMesh, *mesh_box) {
                glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
            }
        }

        // 绘制 pbr 球体
        auto shader_pbr = material_texture ? shader_pbr_texture : shader_pbr_material;
        with(Shader, *shader_pbr) {
            shader_pbr->uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 0.f, -4.f)));
            shader_pbr->uniform_mat4_set("view", camera->view_matrix_get());
            shader_pbr->uniform_mat4_set("projection", camera->projection_matrix_get());
            shader_pbr->uniform_vec3_set("eye_pos", camera->position_get());

            // 光源的属性
            shader_pbr->uniform_vec3_set("light.position", light.position);
            shader_pbr->uniform_vec3_set("light.color", light.color);
            shader_pbr->uniform_vec3_set("ambient", ambient);

            // 物体的参数
            if (material_texture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex_albedo->id);
                shader_pbr->uniform_tex2d_set("texture_albedo", 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, tex_metalness->id);
                shader_pbr->uniform_tex2d_set("texture_metalness", 1);

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, tex_roughness->id);
                shader_pbr->uniform_tex2d_set("texture_alpha", 2);
            } else {
                shader_pbr->uniform_float_set("material.alpha", material.alpha);
                shader_pbr->uniform_float_set("material.metalness", material.metalness);
                shader_pbr->uniform_vec3_set("material.albedo", material.albedo);
                shader_pbr->uniform_float_set("material.ao", material.ao);
            }
            mesh_sphere->draw();
        }
    }

    void _gui() override {
        ImGui::Begin("material");
        ImGui::Checkbox("material use texture", &material_texture);
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

    std::shared_ptr<Shader> shader_pbr_material;
    std::shared_ptr<Shader> shader_light;
    std::shared_ptr<Shader> shader_pbr_texture;

    std::shared_ptr<Texture2D> tex_albedo;
    std::shared_ptr<Texture2D> tex_metalness;
    std::shared_ptr<Texture2D> tex_roughness;

    glm::vec3 ambient{0.2f};

    bool material_texture = true;

    Material material{0.1, 0.9, glm::vec3(0.5), 1.0};
    PLight light{glm::vec3(-4.0f, 1.0f, 4.0f), glm::vec3(300.0f, 300.0f, 300.0f)};
};


int main() {
    run<ScenePbrDL>();
    return 0;
}
