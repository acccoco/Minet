#ifndef RENDER_SCENE_PBR_IBL_H
#define RENDER_SCENE_PBR_IBL_H

#include <memory>
#include "../engine/utils/with.h"
#include "../engine/scene.h"
#include "../engine/shader.h"
#include "../engine/mesh.h"
#include "../engine/camera.h"
#include "../engine/frame_buffer.h"
#include "./obj/box.h"
#include "./obj/sphere.h"


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
        camera = std::make_shared<Camera>();

        shader_sky = std::make_shared<Shader>(SHADER("sky.vert"), SHADER("sky.frag"),
                                              std::vector<std::string>{"HDR_INPUT"});
        shader_ibl_ambient = std::make_shared<Shader>(SHADER("ibl_ambient.vert"), SHADER("ibl_ambient.frag"));
        shader_light = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));

        cubemap_hdr = Texture2D::cubemap_texture_create(512);
        cubemap_env = Texture2D::cubemap_texture_create(512);

        mesh_cube = std::make_shared<PNTMesh>(cube1);
        mesh_sphere = std::make_shared<Sphere>();

        glDepthFunc(GL_LEQUAL);
        hdr2cubemap();
        env_cubemap();
        glViewport(0, 0, Window::width, Window::height);
    }

    void _update() override {
        camera->update();

        // 绘制光源的参考物
        with(Shader, *shader_light) {
            shader_light->uniform_mat4_set("view", camera->view_matrix_get());
            shader_light->uniform_mat4_set("projection", camera->projection_matrix_get());
            shader_light->uniform_mat4_set("model", glm::translate(glm::one<glm::mat4>(), light.position));
            shader_light->uniform_vec3_set("light_color", light.color);
            mesh_cube->draw();
        }

        // 绘制天空盒
        with(Shader, *shader_sky) {
            shader_sky->uniform_mat4_set("view", camera->view_matrix_get());
            shader_sky->uniform_mat4_set("projection", camera->projection_matrix_get());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_hdr);
            shader_sky->uniform_tex2d_set("texture_sky", 0);
            mesh_cube->draw();
        }

        // 绘制球体
        with(Shader, *shader_ibl_ambient) {
            shader_ibl_ambient->uniform_mat4_set("model",
                                                 glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 0.f, -4.f)));
            shader_ibl_ambient->uniform_mat4_set("view", camera->view_matrix_get());
            shader_ibl_ambient->uniform_mat4_set("projection", camera->projection_matrix_get());
            shader_ibl_ambient->uniform_vec3_set("eye_pos", camera->position_get());

            // 光源
            shader_ibl_ambient->uniform_vec3_set("light.position", light.position);
            shader_ibl_ambient->uniform_vec3_set("light.color", light.color);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_env);
            shader_ibl_ambient->uniform_tex2d_set("cubemap_env", 0);

            // 物体的参数
            shader_ibl_ambient->uniform_float_set("material.alpha", material.alpha);
            shader_ibl_ambient->uniform_float_set("material.metalness", material.metalness);
            shader_ibl_ambient->uniform_vec3_set("material.albedo", material.albedo);
            shader_ibl_ambient->uniform_float_set("material.ao", material.ao);

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
    std::shared_ptr<Camera> camera;

    std::shared_ptr<PNTMesh> mesh_cube;
    std::shared_ptr<Sphere> mesh_sphere;

    std::shared_ptr<Shader> shader_sky;
    std::shared_ptr<Shader> shader_ibl_ambient;
    std::shared_ptr<Shader> shader_light;

    Material material{0.1, 0.9, glm::vec3(0.5, 0.0, 0.0), 1.0};
    PLight light{glm::vec3(-4.0f, 1.0f, 4.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

    GLuint cubemap_env;         // 辐照度图
    GLuint cubemap_hdr;         // hdr 贴图转换为立方体贴图

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
        auto shader_hdr2cube = std::make_shared<Shader>(SHADER("hdr2cube.vert"), SHADER("hdr2cube.frag"));
        auto texture_hdr = Texture2D::hdr_load(TEXTURE("Desert_Highway/Road_to_MonumentValley_Ref.hdr"));
        auto frame_buffer = std::make_shared<DepthFrameBuffer>(512, 512);


        with(DepthFrameBuffer, *frame_buffer) {
            with (Shader, *shader_hdr2cube) {
                glViewport(0, 0, 512, 512);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_hdr);

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
        auto shader_convo_env = std::make_shared<Shader>(SHADER("convolution_env.vert"),
                                                         SHADER("convolution_env.frag"));

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


#endif //RENDER_SCENE_PBR_IBL_H
