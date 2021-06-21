#include "model.h"


void Model::move(const glm::vec3 &trans) {
    this->_position += trans;
    this->_model = glm::translate(this->_model, trans);
}

void Model::rotate(const glm::vec3 &axis, float angle) {
    this->_model = glm::rotate(this->_model, angle, axis);
}

void
Model::process_node(std::vector<Mesh> &meshes, const aiNode &node, const aiScene &scene, const std::string &dir_path) {
    /* 处理当前节点 */
    for (unsigned i = 0; i < node.mNumMeshes; ++i) {
        aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
        meshes.push_back(Mesh::mesh_load(*mesh, scene, dir_path));
    }

    /* 处理子节点 */
    for (unsigned i = 0; i < node.mNumChildren; ++i) {
        process_node(meshes, *node.mChildren[i], scene, dir_path);
    }
}

std::shared_ptr<Model> Model::load_model(const std::string &path) {
    auto model = std::make_shared<Model>();
    Assimp::Importer importer;

    SPDLOG_INFO("load model by using Assimp, path: {}", path);

    /* 读取模型，将所有面都处理为三角面，并翻转 UV，如果没有法线，就生成法线 */
    const aiScene *scene = importer.ReadFile(path,
                                             aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_ERROR("error on load model: {}", importer.GetErrorString());
        return model;
    }

    /* 获取模型所在目录的路径，用于读取 texture */
    std::string dir_path = path.substr(0, path.find_last_of('/')) + "/";

    /* 获取所有的 mesh */
    process_node(model->_meshes, *scene->mRootNode, *scene, dir_path);

    return model;
}
