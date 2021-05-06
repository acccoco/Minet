
#include <memory>
#include "../engine/utils/with.h"
#include "../engine/scene.h"
#include "../engine/color.h"
#include "../engine/shader.h"
#include "../engine/model.h"
#include "./obj/cube.h"
#include "./obj/plane.h"
#include "../engine/core.h"


/**
 * 绘制方法：
 * 1. 开启模板缓冲，绘制正常物体
 * 2. 利用之前的模板缓冲，绘制稍微放大的物体轮廓
 */

class SceneEdgeThicken : public Scene {
public:
    void _init() override {

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }

    void _gui() override {
        ImGui::Begin("outline");
        ImGui::SliderFloat("outline scale", &outline_scale, 1, 1.3);
        ImGui::End();
    }

    void _update() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

            /* 绘制普通立方体 */
            glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
            glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
            shader_diffuse->uniform_mat4_set("model", model_box_1);
            mesh_box->draw();

            /* 绘制有边框的立方体 */
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

            model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
            shader_diffuse->uniform_mat4_set("model", model_box_2);
            mesh_box->draw();
        }

        with(Shader, *shader_single_color) {
            shader_single_color->uniform_mat4_set("view", camera->view_matrix_get());
            shader_single_color->uniform_mat4_set("projection", camera->projection_matrix_get());

            /* 绘制外边框 */
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            glm::mat4 model_edge = glm::scale(model_box_2, glm::vec3(outline_scale));
            shader_single_color->uniform_mat4_set("model", model_edge);
            mesh_box->draw();

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glStencilMask(0xFF);
        }


    }

private:
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    glm::mat4 model_box_2{};
    float outline_scale{1.05f};

    std::shared_ptr<MeshT<3, 3, 2>> mesh_box = std::make_shared<MeshT<3, 3, 2>>(cube_pnt);
    std::shared_ptr<MeshT<3, 3, 2>> mesh_floor = std::make_shared<MeshT<3, 3, 2>>(plane_pnt);

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
    std::shared_ptr<Shader> shader_single_color = std::make_shared<Shader>(SHADER("diffuse.vert"),
                                                                           SHADER("single_color.frag"));
};


int main() {
    run<SceneEdgeThicken>();
    return 0;
}
