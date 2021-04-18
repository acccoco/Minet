#ifndef RENDER_SCENE_BOX_FLOOR_H
#define RENDER_SCENE_BOX_FLOOR_H

#include <cmath>
#include <random>
#include <memory>
#include <pthread.h>
#include "../engine/utils/with.h"
#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/utils/with.h"
#include "../config.hpp"
#include "./obj/box.h"
#include "./obj/floor.h"


class SceneBoxFloor : public Scene {
public:

    void _init() override {
        // camera
        camera = std::make_shared<Camera>();

        // mesh
        mesh_box = std::make_shared<PNTMesh>(box_mesh);
        mesh_floor = std::make_shared<PNTMesh>(floor_mesh);

        // shader
        shader_blinn = std::make_shared<Shader>(SHADER("blinn_phong.vert"), SHADER("blinn_phong.frag"));

        // texture
        tex_box = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

        // 光照
        with(Shader, *shader_blinn) {
            shader_blinn->uniform_vec3_set("plight0.pos", glm::vec3(1.f, 1.f, 1.f));
            shader_blinn->uniform_vec3_set("plight0.dir", glm::vec3(0.f, -1.f, 0.f));
            shader_blinn->uniform_vec3_set("plight0.color", Color::color(0xff, 0xe7, 0x42));
        }
    }

    void _update() override {
        camera->update();

        with (Shader, *shader_blinn) {
            shader_blinn->uniform_vec3_set("eye_pos", camera->position_get());
            shader_blinn->uniform_mat4_set("view", camera->view_matrix_get());
            shader_blinn->uniform_mat4_set("projection", camera->projection_matrix_get());

            with(PNTMesh, *mesh_box) {
                glBindTexture(GL_TEXTURE_2D, tex_box->id);
                shader_blinn->uniform_tex2d_set("texture_diffuse_0", 0);

                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, 1.f, 0.f));
                shader_blinn->uniform_mat4_set("model", model);

                glDrawArrays(GL_TRIANGLES, 0, mesh_box->vertex_cnt);
            }

            with(PNTMesh, *mesh_floor) {
                glm::mat4 model = glm::translate(glm::one<glm::mat4>(), glm::vec3(0.f, -1.f, 0.f));
                shader_blinn->uniform_mat4_set("model", model);

                glDrawArrays(GL_TRIANGLES, 0, mesh_floor->vertex_cnt);
            }
        }
    }

private:
    std::shared_ptr<PNTMesh> mesh_box;
    std::shared_ptr<PNTMesh> mesh_floor;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<Shader> shader_blinn;

    std::shared_ptr<Texture2D> tex_box;
};

#endif //RENDER_SCENE_BOX_FLOOR_H
