

#include <memory>

#include "engine/utils/with.h"
#include "engine/scene.h"
#include "engine/shader.h"
#include "engine/render.h"
#include "engine/mesh.h"
#include "engine/camera.h"
#include "engine/frame_buffer.h"

#include "assets/obj/box.h"
#include "assets/obj/sphere.h"

#include "config.hpp"

std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/pbr-image-based-light/{}", EXAMPLE_DIR, file_name);
}


/* PBR：ambient 的图像光照 + diffuse、specular 的直接光照 */
class ScenePbrIBL : public Scene {
    struct Material {
        float alpha;
        float metalness;
        glm::vec3 albedo;
        float ao;
    };

    struct PLight {
        glm::vec3 position;
        glm::vec3 color;
    };


public:
    void _init() override {

        glDepthFunc(GL_LEQUAL);

        SPDLOG_INFO("transform hdr texture -> cube map");
        hdr2cubemap();

        SPDLOG_INFO("calucate irradiance cube map");
        env_cubemap();
        glViewport(0, 0, Window::width(), Window::height());

        /* 数据绑定：shader-sky，用于绘制天空盒 */
        shader_sky->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_sky->set_drawT([](Shader &shader, const Mesh &mesh, const GLuint &texture_id) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
            shader.uniform_tex2d_set("texture_sky", 0);
        });

        /* 数据绑定：shader-light，用于绘制光源 */
        shader_light->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_light->set_drawT([](Shader &shader, const Mesh &mesh, const PLight &light_) {
            shader.uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), light_.position));
            shader.uniform_vec3_set("light_color", light_.color);
        });

        /* 数据绑定：shader-ibl-ambient：用于绘制球体 */
        shader_ibl_ambient->set_update_per_frame([this](Shader &shader){
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
            shader.uniform_vec3_set("eye_pos", Render::camera->position());

            /* 光源 */
            shader.uniform_vec3_set("light.position", this->light.position);
            shader.uniform_vec3_set("light.color", this->light.color);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemap_env);
            shader.uniform_tex2d_set("cubemap_env", 0);

            /* 物体的参数 */
            shader.uniform_float_set("material.alpha", this->material.alpha);
            shader.uniform_float_set("material.metalness", this->material.metalness);
            shader.uniform_vec3_set("material.albedo", this->material.albedo);
            shader.uniform_float_set("material.ao", this->material.ao);
        });
    }

    void _update() override {
        shader_sky->update_per_frame();
        shader_light->update_per_frame();

        // 绘制光源的参考物
        shader_light->draw_t(*mesh_cube, light);

        // 绘制天空盒
        shader_sky->draw_t(*mesh_cube, cubemap_hdr);

        // 绘制球体
        with(Shader, *shader_ibl_ambient) {
            shader_ibl_ambient->uniform_mat4_set("model",
                                                 glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 0.f, -4.f)));
            shader_ibl_ambient->update_per_frame();
            mesh_sphere->draw();
        }
    }

    void _gui() override {
        ImGui::Begin("material");
        ImGui::SliderFloat("alpha", &material.alpha, 0, 1);
        ImGui::SliderFloat("metalness", &material.metalness, 0, 1);
        ImGui::ColorEdit3("albedo", (float *) &material.albedo);
        ImGui::End();

        ImGui::Begin("light");
        ImGui::DragFloat3("position", (float *) &light.position);
        ImGui::ColorEdit3("color", (float *) &light.color);
        ImGui::End();
    }

private:
    std::shared_ptr<Mesh> mesh_cube = std::make_shared<Mesh>(cube1);
    std::shared_ptr<Sphere> mesh_sphere = std::make_shared<Sphere>();

    std::shared_ptr<ShaderT<GLuint>> shader_sky =
            std::make_shared<ShaderT<GLuint>>(CUR_DIR("sky.vert"), CUR_DIR("sky.frag"),
                                              std::vector<std::string>{"HDR_INPUT"});
    std::shared_ptr<Shader> shader_ibl_ambient = std::make_shared<Shader>(CUR_DIR("ibl_ambient.vert"),
                                                                          CUR_DIR("ibl_ambient.frag"));
    std::shared_ptr<ShaderT<PLight>> shader_light = std::make_shared<ShaderT<PLight>>(CUR_DIR("light.vert"),
                                                                                      CUR_DIR("light.frag"));

    Material material{0.1, 0.9, glm::vec3(0.5, 0.0, 0.0), 1.0};
    PLight light{glm::vec3(-4.0f, 1.0f, 4.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

    GLuint cubemap_env = TextureCube::cube_map_create(512);        // 辐照度图
    GLuint cubemap_hdr = TextureCube::cube_map_create(512);         // hdr 贴图转换为立方体贴图

    glm::mat4 views[6] = {
            glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    /* 将等距柱状投影的 hdr 贴图转换为立方体贴图 */
    void hdr2cubemap() {
        auto shader_hdr2cube = std::make_shared<Shader>(CUR_DIR("hdr2cube.vert"), CUR_DIR("hdr2cube.frag"));
        auto texture_hdr = TextureHDR(TEXTURE("Desert_Highway/Road_to_MonumentValley_Ref.hdr"));
        auto frame_buffer = std::make_shared<DepthFrameBuffer>(512, 512);


        with(DepthFrameBuffer, *frame_buffer) {
            with (Shader, *shader_hdr2cube) {
                glViewport(0, 0, 512, 512);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_hdr.id());

                glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
                shader_hdr2cube->uniform_mat4_set("projection", projection);
                shader_hdr2cube->uniform_tex2d_set("texture_hdr", 0);

                for (unsigned int i = 0; i < 6; ++i) {
                    // 绑定帧缓冲，更换颜色 component
                    shader_hdr2cube->uniform_mat4_set("view", views[i]);

                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                           cubemap_hdr, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    mesh_cube->draw();
                }
            }
        }
    }

    /* 根据 hdr 的立方体贴图，通过卷积生成辐照度图 */
    void env_cubemap() {
        auto frame_buffer = std::make_shared<DepthFrameBuffer>(512, 512);
        auto shader_convo_env = std::make_shared<Shader>(CUR_DIR("convolution_env.vert"),
                                                         CUR_DIR("convolution_env.frag"));

        with(DepthFrameBuffer, *frame_buffer) {
            with (Shader, *shader_convo_env) {
                glViewport(0, 0, 512, 512);

                glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_hdr);
                glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
                shader_convo_env->uniform_mat4_set("projection", projection);
                shader_convo_env->uniform_tex2d_set("cubemap_hdr", 0);

                for (unsigned int i = 0; i < 6; ++i) {
                    shader_convo_env->uniform_mat4_set("view", views[i]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                           cubemap_env, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    mesh_cube->draw();
                }

            }
        }
    }
};


int main() {
    Render::init();
    Render::render<ScenePbrIBL>();
    Render::terminate();
    return 0;
}
