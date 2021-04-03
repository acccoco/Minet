#ifndef RENDER_ENGINE_MODEL_H
#define RENDER_ENGINE_MODEL_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <easylogging++.h>

#include "./shader.h"
#include "./mesh.h"
#include "./texture_builder.h"
#include "./mesh_builder.h"


class Model {
public:
    friend class ModelBuilder;

    Model() = default;

    void draw() {
        for (auto &mesh : this->meshes) {
            mesh.draw();
        }
    }

protected:
    std::vector<Mesh> meshes;

};


class Model2 {
protected:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<VAO> mesh;
    glm::mat4 model = glm::one<glm::mat4>();        // 模型矩阵

public:

    Model2() = default;

    virtual void update() = 0;

    std::shared_ptr<Shader> get_shader();

    void update_model_matrix(const glm::vec3 &pos, float angle, const glm::vec3 &axis);

    void translate(const glm::vec3 &move);

    virtual void draw() final;

    glm::mat4 get_model_matrix();

    void bind_shader(const std::shared_ptr<Shader> &shader_);

    /**
     * 为模型绑定 mesh
     */
    void bind_mesh(const std::shared_ptr<VAO> &mesh_);
};

#endif //RENDER_MODEL_H
