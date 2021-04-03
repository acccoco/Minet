#include "../mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
           std::vector<std::shared_ptr<Texture2D>> textures) :
        vertices(std::move(vertices)), faces(faces), textures(std::move(textures)) {
    this->init_texture();
    this->set_up();
}

void Mesh::shader_bind(const std::shared_ptr<Shader> &_shader) {
    this->shader = _shader;
}

void Mesh::draw() {
    this->shader->use();

    // 将 texture 传递给 shader
    for (GLuint i = 0; i < this->texture_map.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, this->texture_map[i].texture->id);
        this->shader->uniform_tex2d_set(this->texture_map[i].name, i);
    }

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, 0);
    glad_glBindVertexArray(0);
}

void Mesh::init_texture() {
    GLuint diffuse_index = 0;
    GLuint specular_index = 0;
    GLuint normal_index = 0;

    for (auto & texture : this->textures) {
        if (texture->type == ETextureType::diffuse) {
            this->texture_map.emplace_back(ETextureType::diffuse_name(diffuse_index++), texture);
        } else if (texture->type == ETextureType::specular) {
            this->texture_map.emplace_back(ETextureType::specular_name(specular_index++), texture);
        } else if (texture->type == ETextureType::normal) {
            this->texture_map.emplace_back(ETextureType::normal_name(normal_index++), texture);
        }
    }
}

void Mesh::set_up() {
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
    glEnableVertexAttribArray(ELocation::position);
    glVertexAttribPointer(ELocation::position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, positon));

    // VAO 顶点属性：normal
    glEnableVertexAttribArray(ELocation::normal);
    glVertexAttribPointer(ELocation::normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, normal));

    // VAO 顶点属性：texcoord
    glEnableVertexAttribArray(ELocation::texcoord);
    glVertexAttribPointer(ELocation::texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex, texcoord));

    // 取消绑定 VAO
    glBindVertexArray(0);
}
