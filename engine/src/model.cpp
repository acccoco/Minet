#include "../model.h"
#include "../shader.h"
#include "../vao.h"

void Model2::bind_shader(const std::shared_ptr<Shader>& shader_) {
    this->shader = shader_;
}

void Model2::bind_mesh(const std::shared_ptr<VAO>& mesh_) {
    this->mesh = mesh_;
}

glm::mat4 Model2::get_model_matrix() {
    return this->model;
}


void Model2::draw() {
    if (this->shader == nullptr || this->mesh == nullptr) return;
    this->shader->use();

    glBindVertexArray(this->mesh->id_get());
    glDrawArrays(GL_TRIANGLES, 0, this->mesh->vertex_cnt_get());

    Shader::unuse();
}

void Model2::update_model_matrix(const glm::vec3 &pos, float angle, const glm::vec3 &axis) {
    static glm::mat4 one = glm::one<glm::mat4>();
    this->model = glm::translate(one, pos);
    this->model = glm::rotate(this->model, angle, axis);
}

std::shared_ptr<Shader> Model2::get_shader() {
    return this->shader;
}


void Model2::translate(const glm::vec3 &move) {
    this->model = glm::translate(this->model, move);
}
