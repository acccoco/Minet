#ifndef RENDER_TEXTURE_BUILDER_H
#define RENDER_TEXTURE_BUILDER_H

#include <vector>
#include <memory>

#include <assimp/scene.h>

#include "./texture.h"
#include "./shader_standard.h"

class Texture2DBuilder {
public:
    static std::vector<std::shared_ptr<Texture2D>>
    build(const std::string &dir, const aiMesh &mesh, const aiScene &scene) {
        std::vector<std::shared_ptr<Texture2D>> textures;

        // 先判断是否有材质
        if (mesh.mMaterialIndex < 0) {
            LOG(INFO) << "this mesh has no material.";
            return textures;
        }

        aiMaterial *material = scene.mMaterials[mesh.mMaterialIndex];

        // diffuse
        auto diffuse_maps = build(dir, *material, aiTextureType_DIFFUSE, ETextureType::diffuse);
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

        // specular
        auto specular_maps = build(dir, *material, aiTextureType_SPECULAR, ETextureType::specular);
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

        return textures;
    }

private:
    static std::vector<std::shared_ptr<Texture2D>>
    build(const std::string &dir, const aiMaterial &mat, aiTextureType type, const std::string &type_name) {
        std::vector<std::shared_ptr<Texture2D>> textures;

        for (unsigned int i = 0; i < mat.GetTextureCount(type); ++i) {
            aiString str;
            std::string full_path = dir + str.C_Str();
            mat.GetTexture(type, i, &str);
            auto texture = std::make_shared<Texture2D>(full_path, type_name);
            textures.push_back(texture);
        }

        return textures;
    }
};

#endif //RENDER_TEXTURE_BUILDER_H
