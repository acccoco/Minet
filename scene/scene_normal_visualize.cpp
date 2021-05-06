
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

#include "../engine/core.h"


class SceneNormalVisualize : public Scene {
public:
    void _init() override {
        /* 创建并绑定 uniform block 对象 */
        glGenBuffers(1, &ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        /* 各个着色器绑定 uniform block */
        for (auto id : std::vector<GLint>{shader_normal->id,
                                          shader_skybox->id,
                                          shader_reflect->id,
                                          shader_normal->id}) {
            glUniformBlockBinding(id, glGetUniformBlockIndex(id, "Matrices"), 0);
        }

    }

    void _gui() override {

    }

    void _update() override {
        camera->update();

        /* 往 unifrom block 中写入内容 */
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->view_matrix_get()));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->projection_matrix_get()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        with(Shader, *shader_diffuse) {
            shader_diffuse->uniform_tex2d_set("texture1", 0);

            /* 绘制地面 */
            glBindTexture(GL_TEXTURE_2D, tex_lava_diffuse->id);
            glm::mat4 model_floor = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, -1.f, 0.f));
            shader_diffuse->uniform_mat4_set("model", model_floor);
            mesh_plane->draw();

            /* 绘制立方体 */
            glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
            glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
            shader_diffuse->uniform_mat4_set("model", model_box_1);
            mesh_cube->draw();
            glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
            shader_diffuse->uniform_mat4_set("model", model_box_2);
            mesh_cube->draw();

        }

        with (Shader, *shader_reflect) {
            shader_diffuse->uniform_tex2d_set("texture1", 0);

            /* 反射效果的盒子 */
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_skybox);
            model_box = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 1.f, 1.f));
            shader_reflect->uniform_mat4_set("model", model_box);
            shader_reflect->uniform_vec3_set("eye_pos", camera->position_get());
            mesh_box->draw();
        }

        with(Shader, *shader_normal) {
            /* 绘制法线效果 */
            shader_normal->uniform_mat4_set("model", model_box);
            mesh_box->draw();
        }

        with(Shader, *shader_skybox) {
            shader_skybox->uniform_int_set("texture_sky", 0);

            /* 绘制天空盒 */
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_skybox);
            mesh_skybox->draw();

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
    }

private:
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    std::shared_ptr<MeshT<3, 3, 2>> mesh_cube = std::make_shared<MeshT<3, 3, 2>>(cube_pnt);
    std::shared_ptr<MeshT<3, 3, 2>> mesh_box = std::make_shared<MeshT<3, 3, 2>>(cube_pnt);
    std::shared_ptr<MeshT<3, 3, 2>> mesh_skybox = std::make_shared<MeshT<3, 3, 2>>(cube_pnt);
    std::shared_ptr<MeshT<3, 3, 2>> mesh_plane = std::make_shared<MeshT<3, 3, 2>>(plane_pnt);

    GLuint ubo_matrices = 0;

    glm::mat4 model_box{};

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"),
                                                                      std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_skybox = std::make_shared<Shader>(SHADER("sky.vert"), SHADER("sky.frag"),
                                                                     std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_reflect = std::make_shared<Shader>(SHADER("reflect.vert"), SHADER("reflect.frag"),
                                                                      std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_normal = std::make_shared<Shader>(SHADER("normal.vert"), SHADER("normal.frag"),
                                                                     std::vector<std::string>{"UNIFORM_BLOCK"},
                                                                     SHADER("normal.geom"));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    GLuint cubemap_skybox = Texture2D::cubemap_tex_create({
                                                                  TEXTURE("skybox/sky_Right.png"),
                                                                  TEXTURE("skybox/sky_Left.png"),
                                                                  TEXTURE("skybox/sky_Up.png"),
                                                                  TEXTURE("skybox/sky_Down.png"),
                                                                  TEXTURE("skybox/sky_Back.png"),
                                                                  TEXTURE("skybox/sky_Front.png")});


};


int main() {
    run<SceneNormalVisualize>();
    return 0;
}
