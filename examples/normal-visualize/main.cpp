
#include <memory>

#include "engine/scene.h"
#include "engine/light.h"
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
    return fmt::format("{}/normal-visualize/{}", EXAMPLE_DIR, file_name);
}


/* 法线可视化 */
class SceneNormalVisualize : public Scene {
public:
    void _init() override {
        /* 创建一个空的 uniform block 对象 */
        glGenBuffers(1, &ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        /* 各个着色器绑定 uniform block */
        for (auto id : {shader_normal->id,
                        shader_skybox->id,
                        shader_reflect->id,
                        shader_normal->id}) {
            glUniformBlockBinding(id, glGetUniformBlockIndex(id, "Matrices"), 0);
        }

        /* 为模型绑定纹理 */
        mesh_floor->add_texture(TextureType::diffuse, tex_lava_diffuse);
        mesh_box->add_texture(TextureType::diffuse, tex_box_diffuse);

        /* 数据绑定：shader-diffuse：mesh */
        shader_diffuse->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.set_textures(mesh, {
                    {"texture1", TextureType::diffuse, 0}
            });
        });

        /* 数据绑定：shader-skybox：mesh */
        shader_reflect->set_update_per_frame([](Shader &shader) {
            shader.uniform_vec3_set("eye_pos", Render::camera->position());
        });
        shader_reflect->set_draw([this](Shader &shader, const Mesh &mesh) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex_skybox.id());
            shader.uniform_int_set("sky_texture", 0);
            shader.uniform_mat4_set("model", mesh.model());
        });

        /* 数据绑定：shader-normal */
        shader_normal->set_draw([](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
        });

        /* 数据绑定：shader_skybox */
        shader_skybox->set_draw([this](Shader &shader, const Mesh &mesh){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex_skybox.id());
            shader.uniform_int_set("texture_sky", 0);
        });
    }

    void _gui() override {}

    void _update() override {
        shader_reflect->update_per_frame();

        /* 往 unifrom block 中写入内容 */
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(Render::camera->view_matrix_get()));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                        glm::value_ptr(Render::camera->projection_matrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        /* 绘制地面 */
        shader_diffuse->draw(*mesh_floor);

        /* 绘制盒子 */
        mesh_box->set_model(model_box_1);
        shader_diffuse->draw(*mesh_box);
        mesh_box->set_model(model_box_2);
        shader_diffuse->draw(*mesh_box);

        /* 绘制具有反射效果的箱子 */
        shader_reflect->draw(*mesh_reflect_box);

        /* 绘制法线效果 */
        shader_normal->draw(*mesh_reflect_box);

        /* 绘制天空盒 */
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        shader_skybox->draw(*mesh_skybox);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

private:

    glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
    glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.f, 0.01f, 2.f));
    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_reflect_box = std::make_shared<Mesh>(cube_pnt_1, glm::vec3(-1.f, 1.f, 1.f));
    std::shared_ptr<Mesh> mesh_skybox = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(plane_pnt, glm::vec3(0.f, -1.f, 0.f));

    GLuint ubo_matrices = 0;

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(CUR_DIR("diffuse.vert"), CUR_DIR("diffuse.frag"),
                                                                      std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_skybox = std::make_shared<Shader>(CUR_DIR("sky.vert"), CUR_DIR("sky.frag"),
                                                                     std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_reflect = std::make_shared<Shader>(CUR_DIR("reflect.vert"), CUR_DIR("reflect.frag"),
                                                                      std::vector<std::string>{"UNIFORM_BLOCK"});
    std::shared_ptr<Shader> shader_normal = std::make_shared<Shader>(CUR_DIR("normal.vert"), CUR_DIR("normal.frag"),
                                                                     std::vector<std::string>{"UNIFORM_BLOCK"},
                                                                     CUR_DIR("normal.geom"));

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    TextureCube tex_skybox{TEXTURE("skybox/sky_Right.png"), TEXTURE("skybox/sky_Left.png"),
                           TEXTURE("skybox/sky_Up.png"), TEXTURE("skybox/sky_Down.png"),
                           TEXTURE("skybox/sky_Back.png"), TEXTURE("skybox/sky_Front.png")};
};


int main() {
    Render::init();
    Render::render<SceneNormalVisualize>();
    Render::terminate();
    return 0;
}
