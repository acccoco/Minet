
#include <memory>

#include "engine/render.h"
#include "engine/scene.h"
#include "engine/mesh.h"
#include "engine/shader.h"

#include "assets/obj/sphere.h"
#include "assets/obj/cube.h"
#include "assets/obj/plane.h"

#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/transparent/{}", EXAMPLE_DIR, file_name);
}


/* 透明效果的场景 */
class SceneTransparent : public Scene {
public:
    void _init() override {
        /* 设置混合函数 */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* 为 mesh 绑定 texture */
        mesh_floor->add_texture(TextureType::diffuse, tex_lava_diffuse);
        mesh_box->add_texture(TextureType::diffuse, tex_box_diffuse);
        mesh_grass->add_texture(TextureType::diffuse, tex_window_transparent);

        /* 数据绑定：shader-diffuse */
        shader_diffuse->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_diffuse->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.set_textures(mesh, {
                    {"texture1", TextureType::diffuse, 0}
            });
        });
    }

    void _gui() override {}

    void _update() override {
        shader_diffuse->update_per_frame();

        /* 绘制地面 */
        shader_diffuse->draw(*mesh_floor);

        /* 绘制 box */
        mesh_box->set_model(model_box_1);
        shader_diffuse->draw(*mesh_box);
        mesh_box->set_model(model_box_2);
        shader_diffuse->draw(*mesh_box);

        /* 渲染半透明窗户 */
        /* 根据到摄像机的距离排序 */
        std::sort(window_positon_list.begin(), window_positon_list.end(),
                  [](const glm::vec3 &a, const glm::vec3 &b) {
                      return glm::length(Render::camera->position() - a) >= glm::length(Render::camera->position() - b);
                  });
        glEnable(GL_BLEND);
        for (auto &window_positon : window_positon_list) {
            mesh_grass->set_position(window_positon);
            shader_diffuse->draw(*mesh_grass);
        }
        glDisable(GL_BLEND);
    }

private:
    std::vector<glm::vec3> window_positon_list = {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3(1.5f, 0.0f, 0.51f),
            glm::vec3(0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3(0.5f, 0.0f, -0.6f)
    };

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(CUR_DIR("diffuse.vert"), CUR_DIR("diffuse.frag"));

    glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
    glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(plane_pnt, glm::vec3(0.f, -1.f, 0.f));
    std::shared_ptr<Mesh> mesh_grass = std::make_shared<Mesh>(plane_pt_3, glm::vec3(), 3, 0, 2);

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    std::shared_ptr<Texture2D> tex_window_transparent = std::make_shared<Texture2D>(TEXTURE("transparent_window.png"),
                                                                                    TextureWrap::CLAMP_TO_EDGE);

};


int main() {
    Render::init();
    Render::render<SceneTransparent>();
    Render::terminate();
    return 0;
}
