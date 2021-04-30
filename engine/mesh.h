#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "./utils/with.h"
#include "./shader.h"
#include "./texture.h"


/* 顶点：坐标，法线，纹理坐标 */
class Vertex {
public:
    glm::vec3 positon{};
    glm::vec3 normal{};
    glm::vec2 texcoord = glm::vec2(0.f, 0.f);

    Vertex() = default;
};


/* 通过 Assimp 来创建 Vertex 对象 */
class VertexBuilder {
public:
    static Vertex build(const aiVector3t<float> &pos, const aiVector3t<float> &norm);

    static Vertex build(const aiVector3t<float> &pos, const aiVector3t<float> &norm, const aiVector3t<float> &uv);

private:
    /* 仅初始化 postition 和 normal */
    static void part_init(Vertex &vertex, const aiVector3t<float> &pos, const aiVector3t<float> &norm);
};


/* 面（顶点索引）*/
class Face {
public:
    unsigned int a;
    unsigned int b;
    unsigned int c;

    Face() = default;

    Face(unsigned int a, unsigned int b, unsigned int c) : a(a), b(b), c(c) {}
};


/* 通过 Assimp 来创建 Face 对象 */
class FaceBuilder {
public:
    static Face build(const aiFace &face);
};


/* 模型：由顶点，面，纹理组成 */
class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures;

    Mesh() = default;

    Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
         std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures);

    GLuint VAO_get() const;

    /* 初始化 Mesh 的各种几何属性，如 VAO，VBO */
    void geometry_init();

    /* 通过 shader，将该 mesh 绘制出来 */
    void draw(const std::shared_ptr<Shader> &shader, GLsizei amount = 1);

private:
    GLuint VAO{}, VBO{}, EBO{};

    /**
     * 将特定类型的 texture 传递给 shader
     * @param unit 纹理单元开始的编号
     */
    void texture_transmit(const std::shared_ptr<Shader> &shader, TextureType type, GLuint &unit);
};


class PNTMesh : public With {
public:
    unsigned int vertex_cnt{};
    GLuint VAO{};

    explicit PNTMesh(const std::vector<float> &vertices);

    void in() override;

    void out() override;

    void draw() const;
};


/* 通过 Assimp 来构建一个 Mesh */
class MeshBuilder {
public:
    static Mesh build(const aiMesh &mesh, const aiScene &scene, const std::string &dir);
};

#endif //RENDER_MESH_H
