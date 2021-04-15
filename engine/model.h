#ifndef RENDER_ENGINE_MODEL_H
#define RENDER_ENGINE_MODEL_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>

#include "./shader.h"
#include "./mesh.h"


/* 一个 Model 由多个 Mesh 组成 */
class Model {
public:
    friend class ModelBuilder;

    Model() = default;
    explicit Model(const glm::vec3 &pos);

    glm::mat4 model_matrix_get();

    std::vector<GLuint> VAOs_get();

    // 改变位姿
    void move(const glm::vec3 &trans);
    void rotate(const glm::vec3 &axis, float angle);

    /**
     * 绘制 model
     * 会更新 model 矩阵，设置 texture 等
     */
    void draw(const std::shared_ptr<Shader> &shader, GLsizei amount = 1);

protected:
    std::vector<Mesh> meshes;
    glm::mat4 model = glm::one<glm::mat4>();            // model 矩阵
    glm::vec3 pos{};                          // Model 的位置
};


/* 通过 Assimp 来创建一个 Model 对象 */
class ModelBuilder {
public:
    static std::shared_ptr<Model> build(const std::string &path);

private:
    static void process_node(Model &model, const aiNode &node, const aiScene &scene, const std::string &dir);
};

#endif //RENDER_MODEL_H
