#ifndef RENDER_MODEL_BUILDER_H
#define RENDER_MODEL_BUILDER_H

#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <easylogging++.h>

#include "./mesh.h"
#include "./model.h"
#include "./mesh_builder.h"


class ModelBuilder {
public:
    static Model build(const std::string &path) {
        Model model;

        Assimp::Importer importer;

        LOG(INFO) << "load model by using Assimp, path: " << path;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        // 检查 scene 是否为空，根节点是否为空
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LOG(ERROR) << "error on load model: " << importer.GetErrorString();
            return model;
        }

        // 获取目录路径
        dir_name = path.substr(0, path.find_last_of('/'));

        // 递归处理每个 mesh
        process_node(model, *scene->mRootNode, *scene);

        return model;
    }

private:
    static std::string dir_name;

    static void process_node(Model &model, const aiNode &node, const aiScene &scene) {
        // 处理当前节点的 mesh
        for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
            aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
            model.meshes.push_back(MeshBuilder::build(*mesh, scene, dir_name));
        }

        // 处理子节点的 mesh
        for (unsigned int i = 0; i < node.mNumChildren; ++i) {
            process_node(model, *node.mChildren[i], scene);
        }
    }
};

#endif //RENDER_MODEL_BUILDER_H
