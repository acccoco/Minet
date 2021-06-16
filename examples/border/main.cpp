
#include <memory>

#include "engine/utils/with.h"
#include "engine/scene.h"
#include "engine/color.h"
#include "engine/shader.h"
#include "engine/model.h"
#include "engine/render.h"

#include "assets/obj/cube.h"
#include "assets/obj/plane.h"

#include "config.hpp"


std::string cur_dir(const std::string &name) {
    return fmt::format("{}/border/{}", EXAMPLE_DIR, name);
}


class SceneEdgeThicken : public Scene {
public:
    void _init() override {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        /* 为模型绑定纹理 */
        mesh_floor->add_texture(TextureType::diffuse, tex_lava_diffuse);
        mesh_box->add_texture(TextureType::diffuse, tex_box_diffuse);

        /* shader-diffuse 数据绑定：mesh */
        shader_diffuse->set_draw([](Shader &shader, const Mesh &mesh) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textures(TextureType::diffuse)[0]->id());
            shader.uniform_tex2d_set("texture1", 0);

            shader.uniform_mat4_set("model", mesh.model());
        });

        /* shader-diffuse 数据绑定：场景相关，每帧 */
        shader_diffuse->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });

        /* shader-single-color 数据绑定：mesh */
        shader_single_color->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
        });

        /* shader-single 数据绑定：每一帧 */
        shader_single_color->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
    }

    void _gui() override {
        ImGui::Begin("outline");
        ImGui::SliderFloat("outline scale", &outline_scale, 1, 1.3);
        ImGui::End();
    }

    // =====================================================
    // 基于模板缓冲实现的物体边框
    // 绘制方法：
    //  1. 开启模板缓冲，绘制正常物体：可以绘制正常正常的物体，还可以得到物体的模版
    //  2. 使用纯色绘制放大的物体，有模版的地方就不绘制
    // =====================================================
    void _update() override {
        shader_diffuse->update_per_frame();
        shader_single_color->update_per_frame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        with(Shader, *shader_diffuse) {

            /* 绘制地面 */
            shader_diffuse->draw(*mesh_floor);

            /* 绘制普通立方体 */
            mesh_box->set_position(position_box_1);
            shader_diffuse->draw(*mesh_box);


            /* 绘制有边框的立方体 */
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

            mesh_box->set_position(position_box_2);
            shader_diffuse->draw(*mesh_box);
        }

        with(Shader, *shader_single_color) {
            /* 绘制外边框 */
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            mesh_box->set_model(glm::scale(glm::translate(glm::one<glm::mat4>(), position_box_2),
                                           glm::vec3(outline_scale)));
            shader_single_color->draw(*mesh_box);

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glStencilMask(0xFF);
        }
    }

private:

    /* 场景中两个箱子的位置 */
    glm::vec3 position_box_1 = glm::vec3(-2.f, 0.01f, -2.f);
    glm::vec3 position_box_2 = glm::vec3(2.f, 0.01f, 2.f);

    /* 边框的粗细 */
    float outline_scale{1.05f};

    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(plane_pnt, glm::vec3(0.f, -1.f, 0.f));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(cur_dir("diffuse.vert"), cur_dir("diffuse.frag"));
    std::shared_ptr<Shader> shader_single_color = std::make_shared<Shader>(cur_dir("diffuse.vert"),
                                                                           cur_dir("single_color.frag"));
};


int main() {
    Render::init();
    Render::render<SceneEdgeThicken>();
    Render::terminate();
    return 0;
}
