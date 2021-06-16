
#include <memory>

#include "engine/render.h"
#include "engine/scene.h"
#include "engine/mesh.h"
#include "engine/shader.h"

#include "assets/obj/sphere.h"
#include "assets/obj/cube.h"
#include "assets/obj/plane.h"

#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/sky-box/{}", EXAMPLE_DIR, file_name);
}


/* 天空盒，透明，折射效果 */
class SceneSkyBox : public Scene {
public:
    void _init() override {
        /* 为 mesh 绑定 texture */
        mesh_floor->add_texture(TextureType::diffuse, tex_lava_diffuse);
        mesh_box->add_texture(TextureType::diffuse, tex_box_diffuse);

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

        /* 数据绑定：shader-reflect*/
        shader_reflect->set_update_per_frame([this](Shader &shader){
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
            shader.uniform_vec3_set("eye_pos", Render::camera->position());
            shader.uniform_float_set("reflect_indensity", reflect_indensity);
        }) ;
        shader_reflect->set_draw([this](Shader &shader, const Mesh &mesh) {
            shader.uniform_mat4_set("model", mesh.model());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->tex_skybox.id());
            shader.uniform_int_set("sky_texture", 0);
        });

        /* 数据绑定：shader-skybox */
        shader_skybox->set_update_per_frame([](Shader &shader){
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_skybox->set_draw([this](Shader &shader, const Mesh &mesh) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->tex_skybox.id());
            shader.uniform_int_set("texture_sky", 0);
        });
    }

    void _gui() override {
        ImGui::Begin("reflect and refract");
        ImGui::SliderFloat("reflect indensity", &reflect_indensity, 0, 1);
        ImGui::End();
    }

    void _update() override {
        shader_diffuse->update_per_frame();
        shader_reflect->update_per_frame();
        shader_skybox->update_per_frame();

        /* 绘制地面 */
        shader_diffuse->draw(*mesh_floor);

        /* 绘制 box */
        mesh_box->set_model(model_box_1);
        shader_diffuse->draw(*mesh_box);
        mesh_box->set_model(model_box_2);
        shader_diffuse->draw(*mesh_box);

        /* 渲染反射效果的盒子 */
        shader_reflect->draw(*mesh_reflect_box);

        /* 渲染天空盒 */
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        shader_skybox->draw(*mesh_skybox);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

private:
    float reflect_indensity = 0.5f;

    std::shared_ptr<Shader> shader_diffuse = std::make_shared<Shader>(CUR_DIR("diffuse.vert"), CUR_DIR("diffuse.frag"));
    std::shared_ptr<Shader> shader_skybox = std::make_shared<Shader>(CUR_DIR("sky.vert"), CUR_DIR("sky.frag"));
    std::shared_ptr<Shader> shader_reflect = std::make_shared<Shader>(CUR_DIR("reflect.vert"), CUR_DIR("reflect.frag"));

    glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.0f, 1.01f, -2.0f));
    glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 1.01f, 0.0f));
    std::shared_ptr<Mesh> mesh_skybox = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_box = std::make_shared<Mesh>(cube_pnt_1);
    std::shared_ptr<Mesh> mesh_reflect_box = std::make_shared<Mesh>(cube_pnt_1, glm::vec3(-2.f, 1.01f, 2.f));
    std::shared_ptr<Mesh> mesh_floor = std::make_shared<Mesh>(plane_pnt);

    std::shared_ptr<Texture2D> tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    std::shared_ptr<Texture2D> tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
    TextureCube tex_skybox = TextureCube(TEXTURE("skybox/sky_Right.png"), TEXTURE("skybox/sky_Left.png"),
                                         TEXTURE("skybox/sky_Up.png"), TEXTURE("skybox/sky_Down.png"),
                                         TEXTURE("skybox/sky_Back.png"), TEXTURE("skybox/sky_Front.png"));
};


int main() {
    Render::init();
    Render::render<SceneSkyBox>();
    Render::terminate();
    return 0;
}
