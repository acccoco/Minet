
#include <memory>

#include "engine/utils/with.h"
#include "engine/scene.h"
#include "engine/color.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/render.h"

#include "assets/obj/box.h"
#include "assets/obj/floor.h"
#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/box-floor/{}", EXAMPLE_DIR, file_name);
}

/* 基础场景：box 和 floor，使用 phong 光照 */
class SceneBoxFloor : public Scene {
public:

    void _init() override {

        /* shader_light 数据绑定，每帧更新 */
        shader_light->set_update_per_frame([this](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
            shader.uniform_vec3_set("light_color", light_color);
        });

        /* shader_light 数据绑定，每 mesh 更新 */
        shader_light->set_draw([this](Shader &, const Mesh &) {
            glm::mat4 model = glm::translate(glm::one<glm::mat4>(), light_pos);
            shader_light->uniform_mat4_set("model", model);
        });

        /* shader_blinn 数据绑定，每帧更新 */
        shader_blinn->set_update_per_frame([this](Shader &shader) {
            shader.uniform_vec3_set("eye_pos", Render::camera->position());
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());

            // 光照模型
            shader.uniform_int_set("blinn_phong", blinn_phong);

            // 光源属性
            shader.uniform_vec3_set("plight0.pos", light_pos);
            shader.uniform_vec3_set("plight0.color", light_color);
        });

        /* shader_blinn 数据绑定，每 mesh 更新 */
        shader_blinn->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.set_textures(mesh, {
                    {"texture_diffuse_0", TextureType::diffuse, 0},
            });
            shader.uniform_mat4_set("model", mesh.model());
        });

        /* 为模型绑定材质 */
        mesh_box->add_texture(TextureType::diffuse, tex_box);
        mesh_floor->add_texture(TextureType::diffuse, tex_floor);
    }

    void _gui() override {
        ImGui::Begin("point light");

        /* 显示帧速率 */
        ImGui::Text("frame rate: %.2f", Render::frame_rate());

        /* 设置灯光的颜色 */
        ImGui::ColorEdit3("light color", (float *) &light_color);

        /* 是否使用 blinn-phong 着色 */
        ImGui::Checkbox("blinn-phong", &blinn_phong);

        /* 设置灯光的位置 */
        ImGui::DragFloat3("light pos", (float *) &light_pos, 0.01f);

        ImGui::End();
    }

    void _update() override {
        shader_light->update_per_frame();
        shader_blinn->update_per_frame();

        // 绘制代表光源的盒子
        with(Shader, *shader_light) {
            shader_light->draw(*mesh_box);
        }

        with (Shader, *shader_blinn) {

            // 绘制箱子
            shader_blinn->draw(*mesh_box);

            // 绘制地面
            shader_blinn->draw(*mesh_floor);
        }
    }

private:
    // 模型
    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(box_mesh, glm::vec3(0.f, 1.f, 0.f));
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(floor_mesh, glm::vec3(0.f, -1.f, 0.f));

    // 着色器
    std::shared_ptr<Shader> shader_blinn = std::make_shared<Shader>(CUR_DIR("blinn_phong.vert"),
                                                                    CUR_DIR("blinn_phong.frag"));
    std::shared_ptr<Shader> shader_light = std::make_shared<Shader>(CUR_DIR("light.vert"), CUR_DIR("light.frag"));

    // 纹理
    std::shared_ptr<Texture2D> tex_box = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    std::shared_ptr<Texture2D> tex_floor = std::make_shared<Texture2D>(TEXTURE("wood_floor.jpg"));

    // gui 参数
    bool blinn_phong = false;
    glm::vec3 light_color = Color::aquamarine2;
    glm::vec3 light_pos{2.f, 1.f, 2.f};
};


int main() {
    Render::init();
    Render::render<SceneBoxFloor>();
    Render::terminate();
    return 0;
}
