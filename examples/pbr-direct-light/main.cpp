#include <memory>
#include <string>
#include "engine/scene.h"
#include "engine/mesh.h"
#include "engine/shader.h"
#include "engine/render.h"

#include "assets/obj/sphere.h"
#include "assets/obj/box.h"

#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/pbr-direct-light/{}", EXAMPLE_DIR, file_name);
}


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

        /* 数据绑定：shader-light */
        shader_light->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_light->set_drawT([](Shader &shader, const Mesh &mesh, const PLight &light_) {
            shader.uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), light_.position));
            shader.uniform_vec3_set("light_color", light_.color);
        });

        /* 数据绑定：shader-pbr-texture */
        shader_pbr_texture->set_update_per_frame([this](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
            shader.uniform_vec3_set("eye_pos", Render::camera->position());

            // 光源的属性
            shader.uniform_vec3_set("light.position", this->light.position);
            shader.uniform_vec3_set("light.color", this->light.color);
            shader.uniform_vec3_set("ambient", this->ambient);

            /* 绑定 texture */
            shader.set_textures({
                                        {"texture_albedo",    tex_albedo->id()},
                                        {"texture_metalness", tex_metalness->id()},
                                        {"texture_alpha",     tex_roughness->id()}
                                });
        });

        /* 数据绑定：shader-pbr-material */
        shader_pbr_material->set_update_per_frame([this](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
            shader.uniform_vec3_set("eye_pos", Render::camera->position());

            // 光源的属性
            shader.uniform_vec3_set("light.position", this->light.position);
            shader.uniform_vec3_set("light.color", this->light.color);
            shader.uniform_vec3_set("ambient", this->ambient);

            /* pbr 相关的属性 */
            shader.uniform_float_set("material.alpha", material.alpha);
            shader.uniform_float_set("material.metalness", material.metalness);
            shader.uniform_vec3_set("material.albedo", material.albedo);
            shader.uniform_float_set("material.ao", material.ao);
        });
    }


    /**
     * shader_pbr_texture 表示 pbr 相关的参数从 texture 中读取
     * shader_pbr_material 表示 pbr 相关的参数是一个数，可以手动设置
     */
    void _update() override {
        shader_light->update_per_frame();

        /* 绘制代表光源的 box */
        shader_light->draw_t(*mesh_box, light);

        // 绘制 pbr 球体
        auto shader_pbr = material_or_texture ? shader_pbr_texture : shader_pbr_material;
        with(Shader, *shader_pbr) {
            shader_pbr->update_per_frame();
            shader_pbr->uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 0.f, -4.f)));
            mesh_sphere->draw();
        }
    }

    void _gui() override {
        ImGui::Begin("material");
        ImGui::Checkbox("material use texture", &material_or_texture);
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
    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(box_mesh);
    std::shared_ptr<Sphere> mesh_sphere = std::make_shared<Sphere>();

    std::shared_ptr<Shader> shader_pbr_material = std::make_shared<Shader>(CUR_DIR("pbr_direct_light.vert"),
                                                                           CUR_DIR("pbr_direct_light.frag"));
    std::shared_ptr<ShaderT<PLight>> shader_light = std::make_shared<ShaderT<PLight>>(CUR_DIR("light.vert"),
                                                                                      CUR_DIR("light.frag"));
    std::shared_ptr<Shader> shader_pbr_texture = std::make_shared<Shader>(CUR_DIR("pbr_direct_light.vert"),
                                                                          CUR_DIR("pbr_direct_light.frag"),
                                                                          std::vector<std::string>{"MATERIAL_TEXTURE"});

    std::shared_ptr<Texture2D> tex_albedo = std::make_shared<Texture2D>(TEXTURE("pbr_ball/rustediron2_basecolor.png"));
    std::shared_ptr<Texture2D> tex_metalness = std::make_shared<Texture2D>(
            TEXTURE("pbr_ball/rustediron2_metallic.png"));
    std::shared_ptr<Texture2D> tex_roughness = std::make_shared<Texture2D>(
            TEXTURE("pbr_ball/rustediron2_roughness.png"));

    glm::vec3 ambient{0.2f};

    bool material_or_texture = true;

    Material material{0.1, 0.9, glm::vec3(0.5), 1.0};
    PLight light{glm::vec3(-4.0f, 1.0f, 4.0f), glm::vec3(300.0f, 300.0f, 300.0f)};
};


int main() {
    Render::init();
    Render::render<ScenePbrDL>();
    Render::terminate();
    return 0;
}
