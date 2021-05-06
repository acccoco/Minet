
#include <memory>
#include <string>
#include "../engine/scene.h"
#include "../engine/mesh.h"
#include "./obj/sphere.h"
#include "./obj/cube.h"
#include "./obj/plane.h"

#include "../engine/core.h"


/* 天空盒，透明，折射效果 */
class SceneSkyBox : public Scene {
public:
    void _init() override {
        camera = std::make_shared<Camera>();

        /* shader */
        shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
        shader_skybox = std::make_shared<Shader>(SHADER("sky.vert"), SHADER("sky.frag"));
        shader_reflect = std::make_shared<Shader>(SHADER("reflect.vert"), SHADER("reflect.frag"));

        /* mesh */
        mesh_skybox = std::make_shared<PNTMesh>(cube_pnt);
        mesh_cube = std::make_shared<PNTMesh>(cube_pnt);
        mesh_box = std::make_shared<PNTMesh>(cube_pnt);
        mesh_floor = std::make_shared<PNTMesh>(plane_pnt);

        /* texture */
        tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
        tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));
        cubemap_skybox = Texture2D::cubemap_tex_create(
                {
                        TEXTURE("skybox/sky_Right.png"),
                        TEXTURE("skybox/sky_Left.png"),
                        TEXTURE("skybox/sky_Up.png"),
                        TEXTURE("skybox/sky_Down.png"),
                        TEXTURE("skybox/sky_Back.png"),
                        TEXTURE("skybox/sky_Front.png")
                });
    }

    void _gui() override {
        ImGui::Begin("reflect and refract");
        ImGui::SliderFloat("reflect indensity", &reflect_indensity, 0, 1);
        ImGui::End();
    }

    void _update() override {
        camera->update();

        /* 渲染地面，立方体盒子 */
        with(Shader, *shader_diffuse) {
            shader_diffuse->uniform_mat4_set("view", camera->view_matrix_get());
            shader_diffuse->uniform_mat4_set("projection", camera->projection_matrix_get());

            /* 绘制地面 */
            shader_diffuse->uniform_mat4_set("model", glm::one<glm::mat4>());
            glBindTexture(GL_TEXTURE_2D, tex_lava_diffuse->id);
            shader_diffuse->uniform_tex2d_set("texture1", 0);
            mesh_floor->draw();

            /* 绘制立方体盒子 1 */
            glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
            shader_diffuse->uniform_tex2d_set("texture1", 0);
            glm::mat4 model_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.0f, 1.01f, -2.0f));
            shader_diffuse->uniform_mat4_set("model", model_1);
            mesh_cube->draw();
            /* 绘制立方体盒子 2 */
            glm::mat4 model_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 1.01f, 0.0f));
            shader_diffuse->uniform_mat4_set("model", model_2);
            mesh_cube->draw();
        }

        /* 渲染反射效果的盒子 */
        with(Shader, *shader_reflect) {
            shader_reflect->uniform_mat4_set("view", camera->view_matrix_get());
            shader_reflect->uniform_mat4_set("projection", camera->projection_matrix_get());
            glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 1.01f, 2.f));
            shader_reflect->uniform_mat4_set("model", model);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_skybox);
            shader_reflect->uniform_int_set("sky_texture", 0);
            shader_reflect->uniform_vec3_set("eye_pos", camera->position_get());
            shader_reflect->uniform_float_set("reflect_indensity", reflect_indensity);
            mesh_box->draw();
        }

        /* 渲染天空盒 */
        with(Shader, *shader_skybox) {
            shader_skybox->uniform_mat4_set("view", camera->view_matrix_get());
            shader_skybox->uniform_mat4_set("projection", camera->projection_matrix_get());

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_skybox);
            shader_skybox->uniform_int_set("texture_sky", 0);
            mesh_skybox->draw();

            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
    }

private:
    float reflect_indensity = 0.5f;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<Shader> shader_diffuse;
    std::shared_ptr<Shader> shader_skybox;
    std::shared_ptr<Shader> shader_reflect;

    std::shared_ptr<PNTMesh> mesh_skybox;
    std::shared_ptr<PNTMesh> mesh_cube;
    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<PNTMesh> mesh_floor;

    std::shared_ptr<Texture2D> tex_lava_diffuse;
    std::shared_ptr<Texture2D> tex_box_diffuse;
    GLuint cubemap_skybox;
};


int main() {
    run<SceneSkyBox>();
    return 0;
}
