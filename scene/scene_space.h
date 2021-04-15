#ifndef RENDER_SCENE_SPACE_H
#define RENDER_SCENE_SPACE_H

#include <cmath>
#include <random>
#include <memory>
#include <pthread.h>
#include "../engine/scene.h"
#include "../engine/light.h"
#include "../engine/color.h"
#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/utils/with.h"
#include "../config.hpp"


class UBOMatrices : public With {
public:
    GLuint id{};

    explicit UBOMatrices(GLuint index) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void in() override {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void out() override {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};

std::vector<glm::mat4> g_model_matrices;
const int core = 16;

void *update_g_model(void *args) {
    int amount = g_model_matrices.size();
    int *task_index = static_cast<int *>(args);
    for (int i = *task_index; i < amount; i += core) {
        g_model_matrices[i] = glm::rotate(g_model_matrices[i], 0.01f, glm::vec3(0.4f, 0.6f, 0.8f));
    }
    return nullptr;
}

class SceneSpace : public Scene {
public:
    void _init() override {
        camera = std::make_shared<Camera>();

        // 模型
        model_planet = ModelBuilder::build(MODEL("planet/planet.obj"));
        model_rock = ModelBuilder::build(MODEL("rock/rock.obj"));

        // 着色器
        shader_planet = std::make_shared<Shader>(SHADER("planet.vert"), SHADER("planet.frag"));
        shader_planet->uniform_block("Matrices", 0);
        shader_rock = std::make_shared<Shader>(SHADER("rock.vert"), SHADER("rock.frag"));
        shader_rock->uniform_block("Matrices", 0);

        // uniform block 初始化
        ubo_matrices = std::make_shared<UBOMatrices>(0);
        with(UBOMatrices, *ubo_matrices) {
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                            glm::value_ptr(camera->projection_matrix_get()));
        }

        // 实例化绘制，初始化 model 矩阵
        init_instance();
    }

    void _update() override {
        camera->update();

        // 通过 ubo 更新 view 矩阵
        with(UBOMatrices, *ubo_matrices) {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->view_matrix_get()));
        }

        shader_rock->uniform_float_set("time", glfwGetTime());

        model_planet->draw(shader_planet);

        // update_models();
        model_rock->draw(shader_rock, amount);
    }

private:
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Model> model_planet;
    std::shared_ptr<Model> model_rock;

    std::shared_ptr<Shader> shader_planet;
    std::shared_ptr<Shader> shader_rock;

    std::shared_ptr<UBOMatrices> ubo_matrices;

    GLuint model_array;
    GLsizei amount = 100000;

private:
    void update_models() const {
        glBindBuffer(GL_ARRAY_BUFFER, model_array);
        pthread_t thread_id[core];
        int task_ids[core];
        for (int i = 0; i < core; ++i) {
            task_ids[i] = i;
            pthread_create(&thread_id[i], nullptr, update_g_model, &task_ids[i]);
        }
        for (pthread_t i : thread_id) {
            pthread_join(i, nullptr);
        }
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &g_model_matrices[0], GL_DYNAMIC_DRAW);
    }

    void gen_models() const {
        float radius = 15.f;            // 半径基准
        float radius_offset = 5.f;

        auto rand_offset = [radius_offset]() {
            return rand() % (int) (2 * radius_offset * 100) / 100.f - radius_offset;
        };

        for (GLsizei i = 0; i < amount; ++i) {
            glm::mat4 model = glm::one<glm::mat4>();

            // 位移
            float angle = (float) i / (float) amount * 360.f;
            float x = std::cos(angle) * radius + rand_offset();
            float y = rand_offset() * 0.04f;
            float z = std::sin(angle) * radius + rand_offset();
            model = glm::translate(model, glm::vec3(x, y, z));

            // 缩放
            float scale = (rand() % 20) / 100.f + 0.05;
            model = glm::scale(model, glm::vec3(scale));

            // 旋转
            float rotate = (rand() % 360);
            model = glm::rotate(model, rotate, glm::vec3(.4f, .6f, .8f));

            g_model_matrices.push_back(model);
        }
    }

    void init_instance() {
        auto VAOs = model_rock->VAOs_get();

        glGenBuffers(1, &model_array);
        glBindBuffer(GL_ARRAY_BUFFER, model_array);
        gen_models();
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &g_model_matrices[0], GL_DYNAMIC_DRAW);

        for (auto VAO: VAOs) {
            glBindVertexArray(VAO);
            for (unsigned int i = 0; i < 4; ++i) {
                glEnableVertexAttribArray(3 + i);
                glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                      (void *) (i * sizeof(glm::vec4)));
                glVertexAttribDivisor(3 + i, 1);        // 这个顶点属性只有在每个实例才更新
            }
            glBindVertexArray(0);
        }
    }
};


#endif //RENDER_SCENE_SPACE_H
