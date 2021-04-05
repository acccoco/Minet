#include "../model.h"
#include "../shader.h"


// Model
// ============================================================================
Model::Model(const glm::vec3 &pos) : pos(pos) {
    this->model = glm::translate(this->model, pos);
}

glm::mat4 Model::model_matrix_get() {
    return this->model;
}

void Model::draw(const std::shared_ptr<Shader> &shader) {
    shader->use();

    // 更新 model 矩阵
    shader->uniform_mat4_set(ShaderMatrixName::model, this->model);

    // 绘制所有的 mesh
    for (auto &mesh : this->meshes) {
        mesh.draw(shader);
    }
}

void Model::move(const glm::vec3 &trans) {
    this->pos += trans;
    this->model = glm::translate(this->model, trans);
}

void Model::rotate(const glm::vec3 &axis, float angle) {
    this->model = glm::rotate(this->model, angle, axis);
}

// ModelBuilder
// ============================================================================
std::shared_ptr<Model> ModelBuilder::build(const std::string &path) {
    auto model = std::make_shared<Model>();
    Assimp::Importer importer;

    SPDLOG_INFO("load model by using Assimp, path: {}", path);

    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    // 检查 scene 是否为空，根节点是否为空
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_ERROR("error on load model: {}", importer.GetErrorString());
        return model;
    }

    // 获取目录路径
    std::string dir_name = path.substr(0, path.find_last_of('/')) + "/";

    // 递归处理每个 mesh
    process_node(*model, *scene->mRootNode, *scene, dir_name);

    return model;
}

void ModelBuilder::process_node(Model &model, const aiNode &node, const aiScene &scene, const std::string &dir) {
    // 处理当前节点的 mesh
    for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
        aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
        model.meshes.push_back(MeshBuilder::build(*mesh, scene, dir));
    }

    // 处理子节点的 mesh
    for (unsigned int i = 0; i < node.mNumChildren; ++i) {
        process_node(model, *node.mChildren[i], scene, dir);
    }
}
