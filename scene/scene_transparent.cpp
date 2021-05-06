
#include <memory>
#include <string>
#include "../engine/scene.h"
#include "../engine/mesh.h"
#include "./obj/sphere.h"
#include "./obj/cube.h"
#include "./obj/plane.h"

#include "../engine/core.h"


class SceneTransparent : public Scene {
public:
    void _init() override {
        /* 设置混合函数 */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
            glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
            model_box_1 = glm::scale(model_box_1, glm::vec3(0.5f));
            shader_diffuse->uniform_mat4_set("model", model_box_1);
            mesh_box->draw();
            glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
            model_box_2 = glm::scale(model_box_2, glm::vec3(0.5f));
            shader_diffuse->uniform_mat4_set("model", model_box_2);
            mesh_box->draw();

            /* 渲染半透明窗户 */
            glEnable(GL_BLEND);
            /* 根据到摄像机的距离排序 */
            std::sort(pos_list_window.begin(), pos_list_window.end(), [this](const glm::vec3 &a, const glm::vec3 &b) {
                return glm::length(camera->position_get() - a) >= glm::length(camera->position_get() - b);
            });
            glBindTexture(GL_TEXTURE_2D, tex_window_transparent->id);
            glm::mat4 model_window;
            for (auto &iter : pos_list_window) {
                model_window = glm::translate(glm::one<glm::mat4>(), iter);
                shader_diffuse->uniform_mat4_set("model", model_window);
                mesh_grass->draw();
            }
            glDisable(GL_BLEND);
        }
    }

private:
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    std::vector<glm::vec3> pos_list_window = {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3(1.5f, 0.0f, 0.51f),
            glm::vec3(0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3(0.5f, 0.0f, -0.6f)
    };

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));

    std::shared_ptr<PNTMesh> mesh_box = std::make_shared<PNTMesh>(cube_pnt);
    std::shared_ptr<PNTMesh> mesh_floor = std::make_shared<PNTMesh>(plane_pnt);
    std::shared_ptr<MeshT<3, 0, 2>> mesh_grass = std::make_shared<MeshT<3, 0, 2>>(plane_pt_3);

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    std::shared_ptr<Texture2D> tex_window_transparent = std::make_shared<Texture2D>(TEXTURE("transparent_window.png"),
                                                                                    false);

};


int main() {
    run<SceneTransparent>();
    return 0;
}
