#include <memory>

#include "engine/scene.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/texture.h"
#include "engine/render.h"

#include "config.hpp"

std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/debugger/{}", ROOT_DIR, file_name);
}

class Ray {
public:
    glm::vec3 pos_start;        /* 光线的起点 */
    glm::vec3 pos_end;          /* 光线的终点 */
};


GLuint get_mesh(const std::vector<Ray> &rays) {

    /* 创建 VAO */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* 根据光线，创建 VBO */
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, rays.size() * sizeof(Ray), &rays[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* 设置顶点属性 */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Ray), (void *) (0 * sizeof(glm::vec3)));
    glVertexAttribDivisor(0, 1);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Ray), (void *) (1 * sizeof(glm::vec3)));
    glVertexAttribDivisor(1, 1);

    /* 恢复 */
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}


class SceneD : public Scene {
    void _init() override {
        /* 模型预处理：缩小一点 */
        auto scale_model = glm::scale(glm::one<glm::mat4>(), {0.01f, 0.01f, 0.01f});
        cornell_box_floor->set_model(scale_model);
        cornell_box_left->set_model(scale_model);
        cornell_box_light->set_model(scale_model);
        cornell_box_right->set_model(scale_model);
        cornell_box_shortbox->set_model(scale_model);
        cornell_box_tallbox->set_model(scale_model);

        /* 数据绑定 shader-diffuse */
        with(Shader, *shader_diffuse) {
            shader_diffuse->uniform_vec3_set("light_dir", {0.f, -1.f, 2.f});
            shader_diffuse->uniform_vec3_set("light_color", {0.6f, 0.6f, 0.6f});
            shader_diffuse->uniform_vec3_set("ambient", {0.4f, 0.4f, 0.4f});
        }
        shader_diffuse->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_diffuse->set_drawT([](Shader &shader, const Model &model, const Mesh &mesh, const glm::vec3 &color) {
            shader.uniform_mat4_set("model", model.model());
            shader.uniform_vec3_set("color", color);
        });

        /* 创建一系列的光线 */
        rays = std::make_shared<Mesh>(std::vector<Line>{
                {{200, 200, 0}, {200, 200, 200}},
                {{300, 300, 0}, {300, 300, 200}}
        });
        rays->set_model(scale_model);       /* 线段的端点都缩放一下 */

        /* 数据绑定：shader-rays */
        shader_rays->set_update_per_frame([](Shader &shader) {
            shader.uniform_mat4_set("view", Render::camera->view_matrix_get());
            shader.uniform_mat4_set("projection", Render::camera->projection_matrix());
        });
        shader_rays->set_drawT([](Shader &shader, const Mesh &mesh, const glm::vec3 &color) {
            shader.uniform_mat4_set("model", mesh.model());
            shader.uniform_vec3_set("ray_color", color);
        });
    }

    void _update() override {
        shader_diffuse->update_per_frame();
        shader_rays->update_per_frame();

        /* 绘制 cornell-box */
        shader_diffuse->draw_t(*cornell_box_floor, {0.725f, 0.71f, 0.68f});
        shader_diffuse->draw_t(*cornell_box_left, {0.63f, 0.065f, 0.05f});
        shader_diffuse->draw_t(*cornell_box_light, {47.8348f, 38.5664f, 31.0808f});
        shader_diffuse->draw_t(*cornell_box_right, {0.14f, 0.45f, 0.091f});
        shader_diffuse->draw_t(*cornell_box_shortbox, {0.725f, 0.71f, 0.68f});
        shader_diffuse->draw_t(*cornell_box_tallbox, {0.725f, 0.71f, 0.68f});

        /* 绘制光线 */
        shader_rays->draw_t(*rays, {1.f, 0.4f, 0.4f});

        /* 绘制坐标轴 */
        shader_rays->draw_t(*mesh_coord_x, {1.f, 0.f, 0.f});
        shader_rays->draw_t(*mesh_coord_y, {0.f, 1.f, 0.f});
        shader_rays->draw_t(*mesh_coord_z, {0.f, 0.f, 1.f});
    }


private:
    /* diffuse 着色的 shader */
    std::shared_ptr<ShaderT<glm::vec3>> shader_diffuse{new ShaderT<glm::vec3>(
            CUR_DIR("diffuse.vert"),
            CUR_DIR("diffuse.frag"))};

    /* 用于渲染线段的 shader */
    std::shared_ptr<ShaderT<glm::vec3>> shader_rays{new ShaderT<glm::vec3>(
            CUR_DIR("rays.vert"),
            CUR_DIR("rays.frag"))
    };

    /* cornell-box 的各个零件 */
    std::shared_ptr<Model> cornell_box_floor = Model::load_model(MODEL("cornell-box/floor.obj"));
    std::shared_ptr<Model> cornell_box_left = Model::load_model(MODEL("cornell-box/left.obj"));
    std::shared_ptr<Model> cornell_box_light = Model::load_model(MODEL("cornell-box/light.obj"));
    std::shared_ptr<Model> cornell_box_right = Model::load_model(MODEL("cornell-box/right.obj"));
    std::shared_ptr<Model> cornell_box_shortbox = Model::load_model(MODEL("cornell-box/shortbox.obj"));
    std::shared_ptr<Model> cornell_box_tallbox = Model::load_model(MODEL("cornell-box/tallbox.obj"));

    /* 场景中的光线 */
    std::shared_ptr<Mesh> rays;

    /* 坐标系的线段 */
    std::shared_ptr<Mesh> mesh_coord_x = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {10.f, 0.f, 0.f}}
    });
    std::shared_ptr<Mesh> mesh_coord_y = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {0.f, 10.f, 0.f}}
    });
    std::shared_ptr<Mesh> mesh_coord_z = std::make_shared<Mesh>(std::vector<Line>{
            {{0.f, 0.f, 0.f}, {0.f, 0.f, 10.f}}
    });
};


int main() {
    Render::init();
    Render::render<SceneD>();
    Render::terminate();
    return 0;
}
