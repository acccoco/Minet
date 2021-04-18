#include "../mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
           std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures) :
        vertices(std::move(vertices)), faces(faces), textures(std::move(textures)) {
    this->geometry_init();
}

void Mesh::draw(const std::shared_ptr<Shader> &shader, GLsizei amount) {

    // 为 shader 设置 texture
    GLuint unit = 0;
    this->texture_transmit(shader, diffuse, unit);
    this->texture_transmit(shader, normal, unit);
    this->texture_transmit(shader, specular, unit);

    // 绘制三角形
    glBindVertexArray(this->VAO);
    if (amount == 1)
        glDrawElements(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    else
        glDrawElementsInstanced(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, nullptr, amount);
    glad_glBindVertexArray(0);
}

void Mesh::geometry_init() {
    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faces.size() * sizeof(Face), &this->faces[0], GL_STATIC_DRAW);

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

    // 取消绑定 VAO
    glBindVertexArray(0);
}

void Mesh::texture_transmit(const std::shared_ptr<Shader> &shader, TextureType type, GLuint &unit) {
    auto iter = this->textures.find(type);
    if (iter == this->textures.end())
        return;

    for (unsigned int i = 0; i < iter->second.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, iter->second[i]->id);
        shader->uniform_tex2d_set(ShaderTextureName::get(type, i), unit);
        ++unit;
    }
}

GLuint Mesh::VAO_get() const {
    return this->VAO;
}

Face FaceBuilder::build(const aiFace &face) {
    assert(face.mNumIndices == 3);

    auto res = Face();
    res.a = face.mIndices[0];
    res.b = face.mIndices[1];
    res.c = face.mIndices[2];

    return res;
}

Vertex VertexBuilder::build(const aiVector3t<float> &pos, const aiVector3t<float> &norm) {
    Vertex vertex;
    part_init(vertex, pos, norm);
    return vertex;
}

Vertex VertexBuilder::build(const aiVector3t<float> &pos, const aiVector3t<float> &norm, const aiVector3t<float> &uv) {
    Vertex vertex;

    part_init(vertex, pos, norm);

    vertex.texcoord.x = uv.x;
    vertex.texcoord.y = uv.y;

    return vertex;
}

void VertexBuilder::part_init(Vertex &vertex, const aiVector3t<float> &pos, const aiVector3t<float> &norm) {
    vertex.positon.x = pos.x;
    vertex.positon.y = pos.y;
    vertex.positon.z = pos.z;

    vertex.normal.x = norm.x;
    vertex.normal.y = norm.y;
    vertex.normal.z = norm.z;
}

Mesh MeshBuilder::build(const aiMesh &mesh, const aiScene &scene, const std::string &dir) {
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures;

    // 处理顶点 position，normal，uv
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        Vertex vertex;
        // 一个顶点可以有多组纹理坐标，这里只需要第一组
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

PNTMesh::PNTMesh(const std::vector<float> &vertices)  {
    assert(vertices.size() % (3 + 3 + 2) == 0);
    vertex_cnt = vertices.size() / (3 + 3 + 2);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PNTMesh::in() {
    glBindVertexArray(VAO);
}

void PNTMesh::out() {
    glBindVertexArray(0);
}
