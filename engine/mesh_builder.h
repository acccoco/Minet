#ifndef RENDER_MESH_BUILDER_H
#define RENDER_MESH_BUILDER_H

#include <vector>
#include <memory>

#include <assimp/scene.h>

#include "./mesh.h"
#include "./texture_builder.h"


class FaceBuilder {
public:
    static Face build(const aiFace &face) {
        assert(face.mNumIndices == 3);

        auto res = Face();
        res.a = face.mIndices[0];
        res.b = face.mIndices[1];
        res.c = face.mIndices[2];
    }
};


class VertexBuilder {
public:
    static Vertex build(const aiVector3t<float> &pos, const aiVector3t<float> &norm) {
        Vertex vertex;
        part_init(vertex, pos, norm);
        return vertex;
    }

    static Vertex build(const aiVector3t<float> &pos, const aiVector3t<float> &norm, const aiVector3t<float> &uv) {
        Vertex vertex;

        part_init(vertex, pos, norm);

        vertex.texcoord.x = uv.x;
        vertex.texcoord.y = uv.y;

        return vertex;
    }

private:
    static void part_init(Vertex &vertex, const aiVector3t<float> &pos, const aiVector3t<float> &norm) {
        vertex.positon.x = pos.x;
        vertex.positon.y = pos.y;
        vertex.positon.z = pos.z;

        vertex.normal.x = norm.x;
        vertex.normal.y = norm.y;
        vertex.normal.z = norm.z;
    }
};


class MeshBuilder {
public:
    static Mesh build(const aiMesh &mesh, const aiScene &scene, const std::string &dir) {
        std::vector<Vertex> vertices;
        std::vector<Face> faces;
        std::vector<std::shared_ptr<Texture2D>> textures;

        // 处理顶点 position，normal，uv
        for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
            Vertex vertex;
            if (mesh.mTextureCoords[0]) {
                vertex = VertexBuilder::build(mesh.mVertices[i], mesh.mNormals[i], mesh.mTextureCoords[0][i]);
            }
            else {
                vertex = VertexBuilder::build(mesh.mVertices[i], mesh.mNormals[i]);
            }

            vertices.push_back(vertex);
        }

        // 处理面
        for (unsigned int i = 0; i < mesh.mNumFaces; ++i) {
            aiFace face = mesh.mFaces[i];
            faces.push_back(FaceBuilder::build(face));
        }

        // 处理材质
        textures = Texture2DBuilder::build(dir, mesh, scene);

        return Mesh(vertices, faces, textures);
    }
};


#endif //RENDER_MESH_BUILDER_H
