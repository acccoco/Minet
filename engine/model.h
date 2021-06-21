#ifndef RENDER_ENGINE_MODEL_H
#define RENDER_ENGINE_MODEL_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "mesh.h"


/* 一个 Model 由多个 Mesh 组成 */
class Model {
public:
    inline Model() : _position{0.f, 0.f, 0.f},
                     _model(glm::one<glm::mat4>()) {}

    inline explicit Model(const glm::vec3 &pos)
            : _position(pos),
              _model(glm::translate(glm::one<glm::mat4>(), pos)) {}

    static std::shared_ptr<Model> load_model(const std::string &path);

    // =====================================================
    // 属性
    // =====================================================

    [[nodiscard]] inline glm::mat4 model() const { return this->_model; }

    inline void set_model(const glm::mat4 &model) { _model = model; }

    [[nodiscard]] inline const std::vector<Mesh> &meshes() const { return _meshes; }


    // =====================================================
    // 改变位姿
    // =====================================================

    void move(const glm::vec3 &trans);

    void rotate(const glm::vec3 &axis, float angle);

protected:
    /* 使用 Assimp 读取模型：递归地读取节点及子节点的模型，将结果放入 meshes 中 */
    static void
    process_node(std::vector<Mesh> &meshes, const aiNode &node, const aiScene &scene, const std::string &dir_path);

protected:

    std::vector<Mesh> _meshes{};
    glm::vec3 _position;                // Model 的位置
    glm::mat4 _model;                   // model 矩阵
};

#endif //RENDER_MODEL_H
