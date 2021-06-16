#include <memory>

#include "engine/render.h"
#include "engine/mesh.h"
#include "engine/scene.h"
#include "engine/color.h"
#include "engine/shader.h"

#include "assets/obj/cube.h"
#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/simple/{}", EXAMPLE_DIR, file_name);
}


/* 简单的只有一个立方体的场景 */
class SceneSimple : public Scene {
private:
    std::shared_ptr<Shader> light_shader = std::make_shared<Shader>(CUR_DIR("light.vert"), CUR_DIR("light.frag"));
    std::shared_ptr<Mesh> mesh_cube = std::make_shared<Mesh>(cube_pnt_0_5, glm::vec3(0.f, 0.f, -5.f));

    void _init() override {

        /* 数据绑定：shader */
        light_shader->uniform_vec3_set("light_color", Color::cyan1);
        light_shader->set_update_per_frame([](Shader &shader){
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        light_shader->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
        });
    }

    void _update() override {
        light_shader->update_per_frame();

        light_shader->draw(*mesh_cube);
    }
};


int main() {
    Render::init();
    Render::render<SceneSimple>();
    Render::terminate();
    return 0;
}
