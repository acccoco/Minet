#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "texture.h"
#include "utils/with.h"


/* 顶点：坐标，法线，纹理坐标 */
class Vertex {
public:
    glm::vec3 positon{0.f, 0.f, 0.f};
    glm::vec3 normal{0.f, 0.f, 0.f};
    glm::vec2 texcoord{0.f, 0.f};

    /* 通过 Assimp 来创建顶点对象 */
    static inline Vertex vertex_gen(const aiVector3D &pos, const aiVector3D &norm) {
        return Vertex{{pos.x,  pos.y,  pos.z},
                      {norm.x, norm.y, norm.z}};
    }

    /* 通过 Assimp 来创建顶点对象 */
    static inline Vertex vertex_gen(const aiVector3D &pos, const aiVector3D &norm, const aiVector3D &uv) {
        return Vertex{{pos.x,  pos.y,  pos.z},
                      {norm.x, norm.y, norm.z},
                      {uv.x,   uv.y}};
    }
};


/* 面（顶点索引）*/
class Face {
public:
    unsigned int a{0};
    unsigned int b{0};
    unsigned int c{0};

    Face(unsigned int a, unsigned int b, unsigned int c) : a(a), b(b), c(c) {}

    /* 通过 Assimp 来创建面 */
    static inline Face face_gen(const aiFace &face) {
        assert(face.mNumIndices == 3);
        return Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }
};


/* 线段 */
class Line {
public:
    Line(const glm::vec3 a, const glm::vec3 b) : _a(a), _b(b) {}

private:
    glm::vec3 _a, _b;
};


/* Mesh 的类型，图元的类型 */
enum class MeshType {
    TriangleArray,          /* 面的顶点是以 array 的形式组织起来的 */
    TriangleElement,        /* 面具有 ebo 索引 */
    Line,                   /* 图元是线段 */
};


/* 模型：由顶点，面，纹理组成 */
class Mesh : With {
public:
    // =====================================================
    // 创建 Mesh 的方法
    // =====================================================

    /* 通过顶点数组，面数组的方式来创建 Mesh，创建的 Mesh 是 Elements 类型的 */
    Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
         std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures,
         const glm::vec3 &position = {0.f, 0.f, 0.f});

    /**
     * 通过顶点数组来创建 mesh
     * @param vertices 顶点的 position，normal，tex_coord 都放在一个数组里面
     * @param position_component 顶点 position 分量有几个元素
     * @param normal_component 顶点 normal 分量有几个元素
     * @param tex_component 顶点 tex_coord 分量有几个元素
     */
    explicit Mesh(const std::vector<float> &vertices, const glm::vec3 &position = {0.f, 0.f, 0.f},
                  int position_component = 3, int normal_component = 3, int tex_component = 2);

    /* 创建由线段组成的模型 */
    explicit Mesh(const std::vector<Line> &lines);


    // 通过 Assimp 来创建 Mesh 对象
    static Mesh mesh_load(const aiMesh &mesh, const aiScene &scene, const std::string &dir);


    // =====================================================
    // 属性
    // =====================================================

    /* 图元的数量 */
    [[nodiscard]] inline GLsizei primitive_cnt() const { return _primitive_cnt; }

    [[nodiscard]] inline GLuint VAO() const { return _vao; }

    [[nodiscard]] inline const glm::mat4 &model() const { return _model_matrix; };

    inline void set_model(const glm::mat4 &model) { _model_matrix = model; }

    inline void set_position(const glm::vec3 &position) {
        _model_matrix = glm::translate(glm::one<glm::mat4>(), position);
    }

    /* 向 Mesh 添加 texture */
    inline void add_texture(TextureType texture_type, const std::shared_ptr<Texture2D> &texture) {
        _textures[texture_type].push_back(texture);
    }

    /* 查找 Mesh 的指定类型的 Texture */
    [[nodiscard]] inline std::vector<std::shared_ptr<Texture2D>> textures(TextureType texture_type) const {
        auto iter = _textures.find(texture_type);
        return iter == _textures.end()
               ? std::vector<std::shared_ptr<Texture2D>>()
               : iter->second;
    }


    inline void in() override { glBindVertexArray(_vao); }

    inline void out() override { glBindVertexArray(0); }

    /* 绘制 Mesh，并不绑定 shader */
    void draw(GLsizei amount = 1) const;

private:
    GLuint _vao{0};
    MeshType _type;
    GLsizei _primitive_cnt{0};
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> _textures;
    glm::mat4 _model_matrix = glm::one<glm::mat4>();

};


#endif //RENDER_MESH_H
