
#include <memory>

#include "engine/scene.h"
#include "engine/color.h"
#include "engine/camera.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/render.h"

#include "config.hpp"
#include "assets/obj/cube.h"
#include "assets/obj/plane.h"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/face-cull/{}", EXAMPLE_DIR, file_name);
}


/* 背面剔除 */
class SceneFaceCull : public Scene {
public:
    void _init() override {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        /* 绑定纹理 */
        mesh_box->add_texture(TextureType::diffuse, tex_box_diffuse);
        mesh_floor->add_texture(TextureType::diffuse, tex_lava_diffuse);

        /* shader-diffuse 数据绑定：每帧，场景 */
        shader_diffuse->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });

        /* shader-diffuse 数据绑定：mesh */
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

        with(Shader, *shader_diffuse) {

            /* 绘制地面 */
            shader_diffuse->draw(*mesh_floor);

            /* 绘制立方体 */
            mesh_box->set_model(model_box_1);
            shader_diffuse->draw(*mesh_box);
            mesh_box->set_model(model_box_2);
            shader_diffuse->draw(*mesh_box);
        }
    }

private:
    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(CUR_DIR("diffuse.vert"), CUR_DIR("diffuse.frag"));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(plane_pnt, glm::vec3(0.f, -1.f, 0.f));

    glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
    glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
};


int main() {
    Render::init();
    Render::render<SceneFaceCull>();
    Render::terminate();
    return 0;
}
