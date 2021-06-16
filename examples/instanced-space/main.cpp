
#include <cmath>
#include <random>
#include <memory>
#include <pthread.h>

#include <fmt/format.h>

#include "engine/render.h"
#include "engine/scene.h"
#include "engine/light.h"
#include "engine/color.h"
#include "engine/camera.h"
#include "engine/model.h"
#include "engine/shader.h"
#include "engine/texture.h"

#include "engine/utils/with.h"
#include "config.hpp"


std::string CUR_DIR(const std::string &file_name) {
    return fmt::format("{}/instanced-space/{}", EXAMPLE_DIR, file_name);
}


/* unform block 的类型 */
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


const int core = 16;


class SceneSpace : public Scene {
public:
    void _init() override {
        /* 数据绑定：shader-planet */
        shader_planet->uniform_block("Matrices", 0);
        shader_planet->set_draw([](Shader &shader, const Model &model, const Mesh &mesh) {
            shader.uniform_mat4_set("model", model.model());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textures(TextureType::diffuse)[0]->id());
            shader.uniform_tex2d_set("material.texture_diffuse_0", 0);
        });

        /* 数据绑定：shader-rock */
        shader_rock->uniform_block("Matrices", 0);
        shader_rock->set_draw([](Shader &shader, const Model &model, const Mesh &mesh) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textures(TextureType::diffuse)[0]->id());
            shader.uniform_tex2d_set("material.texture_diffuse_0", 0);
        });

        // uniform block 初始化
        ubo_matrices = std::make_shared<UBOMatrices>(0);
        with(UBOMatrices, *ubo_matrices) {
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                            glm::value_ptr(Render::camera->projection_matrix()));
        }

        // 实例化绘制，初始化 model 矩阵
        GLuint model_array = init_instance(amount);
        glBindBuffer(GL_ARRAY_BUFFER, model_array);
        for (const Mesh &mesh: model_rock->meshes()) {
            glBindVertexArray(mesh.VAO());
            /* matrix4 类型的需要四个顶点属性来存储 */
            for (unsigned int i = 0; i < 4; ++i) {
                glEnableVertexAttribArray(3 + i);
                glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                      (void *) (i * sizeof(glm::vec4)));
                glVertexAttribDivisor(3 + i, 1);        // 这个顶点属性只有在每个实例才更新
            }
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void _update() override {

        // 通过 ubo 更新 view 矩阵
        with(UBOMatrices, *ubo_matrices) {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(Render::camera->view_matrix_get()));
        }

        /* 绘制 rock */
        with(Shader, *shader_rock) {
            shader_rock->uniform_float_set("time", (float) glfwGetTime());
            shader_rock->draw(*model_rock, nullptr, amount);
        }

        /* 绘制 planet */
        with(Shader, *shader_planet) {
            shader_planet->draw(*model_planet);
        }
    }

private:

    std::shared_ptr<Model> model_planet = Model::load_model(MODEL("planet/planet.obj"));
    std::shared_ptr<Model> model_rock = Model::load_model(MODEL("rock/rock.obj"));

    std::shared_ptr<Shader> shader_planet = std::make_shared<Shader>(CUR_DIR("planet.vert"), CUR_DIR("planet.frag"));
    std::shared_ptr<Shader> shader_rock = std::make_shared<Shader>(CUR_DIR("rock.vert"), CUR_DIR("rock.frag"));

    std::shared_ptr<UBOMatrices> ubo_matrices;

    GLsizei amount = 1000;

private:

    /* 生成 model 矩阵 */
    static std::vector<glm::mat4> gen_models(GLsizei amount) {
        std::vector<glm::mat4> model_matrices;

        float radius = 15.f;            // 半径基准
        float radius_offset = 5.f;

        auto rand_offset = [radius_offset]() {
            return rand() % (int) (2 * radius_offset * 100) / 100.f - radius_offset;
        };

        for (GLsizei i = 0; i < amount; ++i) {
            auto model = glm::one<glm::mat4>();

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

            model_matrices.push_back(model);
        }
        return model_matrices;
    }

    /* 生成很多 model matrix，并将其作为 GL_ARRAY_BUFFER */
    static GLuint init_instance(GLsizei amount) {
        GLuint model_array;

        /* 创建一个 array buffer，存放 instance 的位置信息 */
        glGenBuffers(1, &model_array);
        glBindBuffer(GL_ARRAY_BUFFER, model_array);
        auto model_matrices = gen_models(amount);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &model_matrices[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return model_array;
    }
};


int main() {
    Render::init();
    Render::render<SceneSpace>();
    Render::terminate();
    return 0;
}
