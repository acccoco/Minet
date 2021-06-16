
#include <memory>

#include "engine/scene.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/render.h"

#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/nano-suit/{}", EXAMPLE_DIR, file_name);
}


/* 纳米装甲模型的场景 */
class SceneNano : public Scene {
private:
    std::shared_ptr<Model> model_nano = Model::load_model(MODEL("nanosuit/nanosuit.obj"));
    std::shared_ptr<Shader> tex_shader = std::make_shared<Shader>(CUR_DIR("tex.vert"),
                                                                  CUR_DIR("tex.frag"));

    void _init() override {
        /* shader 和 mesh 的数据绑定 */
        tex_shader->set_draw([](Shader &shader, const Model &model, const Mesh &mesh) {
            shader.uniform_mat4_set("model", model.model());
            shader.set_textures(mesh, {
                    {"material.texture_diffuse_0",  TextureType::diffuse,  0},
                    {"material.texture_specular_0", TextureType::specular, 0},
            });
        });

        /* 场景数据绑定：tex_shader */
        tex_shader->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
    }

    void _update() override {
        tex_shader->update_per_frame();

        with(Shader, *tex_shader) {
            tex_shader->draw(*model_nano);
        }
    }
};


int main() {
    Render::init();
    Render::render<SceneNano>();
    Render::terminate();
    return 0;
}
