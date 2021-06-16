
#include <memory>

#include "engine/utils/with.h"
#include "engine/scene.h"
#include "engine/shader.h"
#include "engine/mesh.h"
#include "engine/camera.h"
#include "engine/frame_buffer.h"
#include "engine/frame_buffer.h"
#include "engine/render.h"

#include "assets/obj/cube.h"
#include "assets/obj/plane.h"
#include "assets/obj/sphere.h"

#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/post-process/{}", EXAMPLE_DIR, file_name);
}


/* 基于帧缓冲的后期处理，使用了卷积核 */
class ScenePostProcess : public Scene {
public:
    void _init() override {
        /* 为 mesh 绑定 texture */
        mesh_plane->add_texture(TextureType::diffuse, tex_lava_diffuse);
        mesh_cube->add_texture(TextureType::diffuse, tex_box_diffuse);

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

        /* 数据绑定 */
        shader_framebuffer->set_draw([this](Shader &shader, const Mesh &mesh){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->framebuffer->color_tex_get());
            shader.uniform_tex2d_set("texture1", 0);
            shader.uniform_int_set("post_process_id", post_process);
        });
    }

    void _gui() override {
        ImGui::Begin("post process");
        static std::string comment = "0: inverse\n"
                                     "1: gray\n"
                                     "2: sharp1\n"
                                     "3: sharp2\n"
                                     "4: blur\n"
                                     "5: edge detect\n"
                                     "6: edge detect + gray\n"
                                     "others: original color";
        ImGui::SliderInt("post process", &post_process, 0, 7);
        ImGui::Text("%s", comment.c_str());
        ImGui::End();
    }


    // =====================================================
    // 后处理的原理：
    //  1. 首先绑定一个自定义的 framebuffer，渲染场景
    //  2. 将 framebuffer 作为 texture，使用后处理着色器，渲染到 screen 上面
    // =====================================================

    void _update() override {

        /* 在 framebuffer 中绘制场景 */
        with(FrameBuffer, *framebuffer) {
            shader_diffuse->update_per_frame();

            glEnable(GL_DEPTH_TEST);
            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* 绘制地面 */
            shader_diffuse->draw(*mesh_plane);

            /* 绘制立方体 */
            mesh_cube->set_model(model_box_1);
            shader_diffuse->draw(*mesh_cube);
            mesh_cube->set_model(model_box_2);
            shader_diffuse->draw(*mesh_cube);
        }

        /* 使用正方形渲染 framebuffer，做后处理 */
        glDisable(GL_DEPTH_TEST);
        shader_framebuffer->draw(*mesh_square);
    }


private:

    int post_process = 0;

    // todo 动态调节尺寸
    std::shared_ptr<FrameBuffer> framebuffer = std::make_shared<FrameBuffer>(2560, 1440);

    glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
    glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 1.0f));
    std::shared_ptr<Mesh> mesh_cube = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_plane = std::make_shared<Mesh>(plane_pnt, glm::vec3(0.f, -1.f, 0.f));
    std::shared_ptr<Mesh> mesh_square = std::make_shared<Mesh>(plane_pt_2, glm::vec3(), 2, 0, 2);

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(CUR_DIR("diffuse.vert"), CUR_DIR("diffuse.frag"));
    std::shared_ptr<Shader> shader_framebuffer = std::make_shared<Shader>(CUR_DIR("framebuffer.vert"),
                                                                          CUR_DIR("framebuffer.frag"));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

};


int main() {
    Render::init();
    Render::render<ScenePostProcess>();
    Render::terminate();
    return 0;
}
