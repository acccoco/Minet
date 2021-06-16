#include <stdexcept>
#include "mesh.h"
#include "global.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
           std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures,
           const glm::vec3 &position) :
        _type(MeshType::Elements),
        _face_cnt((GLsizei) faces.size()),
        _textures(std::move(textures)) {

    /* 设置 mesh 的位置 */
    _model_matrix = glm::translate(glm::one<glm::mat4>(), position);

    // VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizei(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);

    // EBO
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizei(faces.size() * sizeof(Face)), &faces[0], GL_STATIC_DRAW);

    // VAO 顶点属性：position
    glEnableVertexAttribArray(VertAttribLocation::position);
    glVertexAttribPointer(VertAttribLocation::position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, positon));

    // VAO 顶点属性：normal
    glEnableVertexAttribArray(VertAttribLocation::normal);
    glVertexAttribPointer(VertAttribLocation::normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, normal));

    // VAO 顶点属性：texcoord
    glEnableVertexAttribArray(VertAttribLocation::texcoord);
    glVertexAttribPointer(VertAttribLocation::texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, texcoord));

    // 取消绑定
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


Mesh Mesh::mesh_load(const aiMesh &mesh, const aiScene &scene, const std::string &dir) {
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures;

    // 处理顶点
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        // 一个顶点可以有多组纹理坐标，这里只需要第一组
        if (mesh.mTextureCoords[0]) {
            vertices.push_back(Vertex::vertex_gen(mesh.mVertices[i], mesh.mNormals[i], mesh.mTextureCoords[0][i]));
        } else {
            vertices.push_back(Vertex::vertex_gen(mesh.mVertices[i], mesh.mNormals[i]));
        }
    }

    // 处理面
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i) {
        faces.push_back(Face::face_gen(mesh.mFaces[i]));
    }

    // 处理材质
    if (mesh.mMaterialIndex < 0) {
        SPDLOG_INFO("this mesh has no material.");
    } else {
        aiMaterial *material = scene.mMaterials[mesh.mMaterialIndex];
        textures = TextureManager::textures_get(*material, dir);
    }

    return Mesh(vertices, faces, textures);
}


Mesh::Mesh(const std::vector<float> &vertices, const glm::vec3 &position,
           int position_component, int normal_component, int tex_component)
        : _type(MeshType::Array) {

    /* 设置模型的位置 */
    _model_matrix = glm::translate(glm::one<glm::mat4>(), position);

    /* position + normal + tex_coord 一共有几个分量 */
    const int all_component = position_component + normal_component + tex_component;
    assert(all_component > 0);
    assert(vertices.size() % (all_component * 3) == 0);
    _face_cnt = GLsizei(vertices.size() / all_component / 3);

    /* VAO */
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    /* VBO */
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(float)), &vertices[0], GL_STATIC_DRAW);

    /* 顶点属性 */
    if (position_component != 0) {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, (GLint) position_component, GL_FLOAT, GL_FALSE, GLsizei(all_component * sizeof(float)),
                              (void *) nullptr);
    }
    if (normal_component != 0) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, (GLint) normal_component, GL_FLOAT, GL_FALSE, GLsizei(all_component * sizeof(float)),
                              (void *) (position_component * sizeof(float)));
    }
    if (tex_component != 0) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, (GLint) tex_component, GL_FLOAT, GL_FALSE, GLsizei(all_component * sizeof(float)),
                              (void *) ((position_component + normal_component) * sizeof(float)));
    }

    /* 解除绑定 */
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw(GLsizei amount) const {
    assert(_face_cnt != 0);
    glBindVertexArray(this->_vao);
    switch (_type) {
        case MeshType::Elements:
            if (amount == 1)
                glDrawElements(GL_TRIANGLES, _face_cnt * 3, GL_UNSIGNED_INT, nullptr);
            else
                glDrawElementsInstanced(GL_TRIANGLES, _face_cnt * 3, GL_UNSIGNED_INT, nullptr, amount);
            break;
        case MeshType::Array:
            glDrawArrays(GL_TRIANGLES, 0, _face_cnt * 3);
            break;
        default:
            throw std::runtime_error("never");
    }
    glBindVertexArray(0);
}

