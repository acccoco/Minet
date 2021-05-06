
#include <memory>

#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"

#include "../config.hpp"
#include "obj/cube.h"
#include "obj/plane.h"

#include "../engine/core.h"//


/* 背面剔除 */
class SceneFaceCull : public Scene {
public:
    void _init() override {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void _gui() override {

    }

    void _update() override {
        camera->update();

        with(Shader, *shader_diffuse) {
            shader_diffuse->uniform_mat4_set("view", camera->view_matrix_get());
            shader_diffuse->uniform_mat4_set("projection", camera->projection_matrix_get());
            shader_diffuse->uniform_tex2d_set("texture1", 0);

            /* 绘制地面 */
            glBindTexture(GL_TEXTURE_2D, tex_lava_diffuse->id);
            glm::mat4 model_floor = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, -1.f, 0.f));
            shader_diffuse->uniform_mat4_set("model", model_floor);
            mesh_floor->draw();

            /* 绘制立方体 */
            glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
            glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
            glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
            shader_diffuse->uniform_mat4_set("model", model_box_1);
            mesh_box->draw();
            shader_diffuse->uniform_mat4_set("model", model_box_2);
            mesh_box->draw();
        }
    }

private:
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

    std::shared_ptr<MeshT<3, 3, 2>> mesh_box = std::make_shared<MeshT<3, 3, 2>>(cube_pnt);
    std::shared_ptr<MeshT<3, 3, 2>> mesh_floor = std::make_shared<MeshT<3, 3, 2>>(plane_pnt);
};


int main() {
    run<SceneFaceCull>();
    return 0;
}
