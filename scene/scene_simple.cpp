
#include "../engine/scene.h"
#include "../engine/color.h"

#include "obj/light_box.h"

#include "../engine/core.h"


/* 简单的只有一个立方体的场景 */
class SceneSimple : public Scene {
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Model> model;
    std::shared_ptr<Camera> camera;

    void _init() override {
        // 初始化摄像机
        this->camera = std::make_shared<Camera>();

        // 初始化着色器
        this->shader = std::make_shared<Shader>(SHADER("light.vert"), SHADER("light.frag"));
        this->shader->uniform_vec3_set("light_color", Color::cyan1);

        // 初始化模型
        this->model = std::make_shared<LightBox>();
        this->model->move(glm::vec3(0.f, 0.f, -5.f));
    }

    void _update() override {
        // 更新相机
        this->camera->update();

        this->shader->use();
        this->shader->uniform_mat4_set("view", this->camera->view_matrix_get());
        this->shader->uniform_mat4_set("projection", this->camera->projection_matrix_get());
        this->model->draw(this->shader);
        glUseProgram(0);
    }
};


int main() {
    run<SceneSimple>();
    return 0;
}
