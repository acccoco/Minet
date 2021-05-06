
#include <memory>
#include "../engine/utils/with.h"
#include "../engine/scene.h"
#include "../engine/shader.h"
#include "../engine/mesh.h"
#include "../engine/camera.h"
#include "../engine/frame_buffer.h"
#include "../engine/frame_buffer.h"
#include "./obj/cube.h"
#include "./obj/plane.h"
#include "./obj/sphere.h"

#include "../engine/core.h"


/* 基于帧缓冲的后期处理，使用了卷积核 */
class ScenePostProcess : public Scene {
public:
    void _init() override {
        camera = std::make_shared<Camera>();

        /* shader */
        shader_diffuse = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
        shader_framebuffer = std::make_shared<Shader>(SHADER("framebuffer.vert"), SHADER("framebuffer.frag"));

        /* texture */
        tex_lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
        tex_box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

        /* framebuffer */
        framebuffer = std::make_shared<FrameBuffer>(1600, 1200);

        /* mesh */
        mesh_square = std::make_shared<PT2Mesh>(plane_pt_2);
        mesh_cube = std::make_shared<PNTMesh>(cube_pnt);
        mesh_plane = std::make_shared<PNTMesh>(plane_pnt);
    }

    void _gui() override {
        ImGui::Begin("post process");
        static std::string comment = "0: inverse\n"
                                     "1: gray\n"
                                     "2: sharp1\n"
                                     "3: sharp2\n"
                                     "4: blur\n"
                                     "5: edge detect\n"
                                     "6: edge detect + gray\n"
                                     "others: original color" ;

        ImGui::SliderInt("post process", &post_process, 0, 7);
        ImGui::Text("%s", comment.c_str());
        ImGui::End();
    }

    void _update() override {
        camera->update();

        /* 在 framebuffer 中绘制场景 */
        with(FrameBuffer, *framebuffer) {
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            with (Shader, *shader_diffuse) {
                shader_diffuse->uniform_mat4_set("view", camera->view_matrix_get());
                shader_diffuse->uniform_mat4_set("projection", camera->projection_matrix_get());

                /* 绘制地面 */
                glBindTexture(GL_TEXTURE_2D, tex_lava_diffuse->id);
                shader_diffuse->uniform_tex2d_set("texture1", 0);
                glm::mat4 model_floor = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, -1.f, 0.f));
                shader_diffuse->uniform_mat4_set("model", model_floor);
                mesh_plane->draw();

                /* 绘制立方体 */
                glBindTexture(GL_TEXTURE_2D, tex_box_diffuse->id);
                shader_diffuse->uniform_tex2d_set("texture1", 0);
                glm::mat4 model_box_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-2.f, 0.01f, -2.f));
                shader_diffuse->uniform_mat4_set("model", model_box_1);
                mesh_cube->draw();
                glm::mat4 model_box_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 1.0f));
                shader_diffuse->uniform_mat4_set("model", model_box_2);
                mesh_cube->draw();
            }
        }

        /* 使用正方形渲染 framebuffer，做后处理 */
        glDisable(GL_DEPTH_TEST);
        with(Shader, *shader_framebuffer) {
            glBindTexture(GL_TEXTURE_2D, framebuffer->color_tex_get());
            shader_framebuffer->uniform_tex2d_set("texture1", 0);
            shader_framebuffer->uniform_int_set("pp", post_process);
            mesh_square->draw();
        }
    }


private:
    std::shared_ptr<Camera> camera;

    int post_process = 0;

    std::shared_ptr<FrameBuffer> framebuffer;

    std::shared_ptr<PNTMesh> mesh_cube;
    std::shared_ptr<PNTMesh> mesh_plane;
    std::shared_ptr<PT2Mesh> mesh_square;

    std::shared_ptr<Shader> shader_diffuse;
    std::shared_ptr<Shader> shader_framebuffer;

    std::shared_ptr<Texture2D> tex_lava_diffuse;
    std::shared_ptr<Texture2D> tex_box_diffuse;

};


int main() {
    run<ScenePostProcess>();
    return 0;
}
