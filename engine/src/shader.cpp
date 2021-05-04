
#include <cassert>
#include <exception>
#include "../shader.h"
#include "../utils/file.h"


// 全局变量 =======================================================================
const int LOG_INFO_LEN = 512;


const GLuint VertAttribLocation::position = 0;
const GLuint VertAttribLocation::normal = 1;
const GLuint VertAttribLocation::texcoord = 2;


const std::string ShaderMatrixName::model = "model";
const std::string ShaderMatrixName::view = "view";
const std::string ShaderMatrixName::projection = "projection";


// 类方法实现 ======================================================================
GLint Shader::unifrom_location_get(const std::string &name) {
    auto iter = uniform_location_map.find(name);

    // 没找到，需要调用 OpenGL 的接口查询
    if (iter == uniform_location_map.end()) {
        int location = glGetUniformLocation(this->id, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("fail to find shader uniform: {}", name);
            throw (std::exception());
        }
        uniform_location_map.insert({name, location});
        return location;
    }

    // 找到了
    return iter->second;
}


Shader::Shader(const std::string &vertex, const std::string &fragment, const std::vector<std::string> &macros) {
    GLuint id_vertex;
    GLuint id_fragment;

    /* 编译着色器 */
    id_vertex = shader_compile(vertex, GL_VERTEX_SHADER, macros);
    id_fragment = shader_compile(fragment, GL_FRAGMENT_SHADER, macros);

    /* 链接着色器 */
    this->id = shader_link(id_vertex, id_fragment);

    /* 删除着色器对象 */
    glDeleteShader(id_vertex);
    glDeleteShader(id_fragment);
}


GLuint
Shader::shader_compile(const std::string &file_name, GLenum shader_type, const std::vector<std::string> &macros) {
    assert(shader_type == GL_VERTEX_SHADER || shader_type == GL_FRAGMENT_SHADER);

    /* 读文件 */
    std::vector<std::string> lines = File::file_load_lines(file_name);
    std::string shader_source;
    for (const auto &line : lines) {
        shader_source += line;

        /* 在 #version 后面追加宏定义 */
        if (line.find("#version") != std::string::npos) {
            for (auto const &macro : macros) {
                shader_source += "#define " + macro + "\n";
            }
        }
    }
    const char *source = shader_source.c_str();

    // 编译
    SPDLOG_INFO("compile shader");
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // 获取编译结果
    int success;
    char log_info[LOG_INFO_LEN];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, LOG_INFO_LEN, nullptr, log_info);
        SPDLOG_ERROR("fail to compile shader, info log\n {}", log_info);
        throw std::exception();
    }

    return shader;
}

void Shader::use() const {
    glUseProgram(this->id);
}

void Shader::uniform_vec4_set(const std::string &name, const glm::vec4 &v) {
    glUseProgram(this->id);
    glUniform4f(unifrom_location_get(name), v.x, v.y, v.z, v.w);
}

void Shader::uniform_float_set(const std::string &name, float value) {
    glUseProgram(this->id);
    glUniform1f(unifrom_location_get(name), value);
}

void Shader::uniform_int_set(const std::string &name, GLint value) {
    glUseProgram(this->id);
    glUniform1i(unifrom_location_get(name), value);
}

void Shader::uniform_vec3_set(const std::string &name, const glm::vec3 &v) {
    glUseProgram(this->id);
    glUniform3f(unifrom_location_get(name), v.x, v.y, v.z);
}

void Shader::uniform_mat4_set(const std::string &name, const glm::mat4 &m) {
    glUseProgram(this->id);
    glUniformMatrix4fv(unifrom_location_get(name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::uniform_tex2d_set(const std::string &name, GLuint texture_unit) {
    glUseProgram(this->id);
    glUniform1i(unifrom_location_get(name), texture_unit);
}

void Shader::uniform_block(const std::string &name, GLuint index) const {
    GLuint uniform_block_location = glGetUniformBlockIndex(id, name.c_str());
    glUniformBlockBinding(id, uniform_block_location, index);
}

void Shader::in() {
    glUseProgram(this->id);
}

void Shader::out() {
    glUseProgram(0);
}

GLuint Shader::shader_link(GLuint vertex, GLuint fragment) {
    // 链接着色器
    SPDLOG_INFO("link shader");
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    glLinkProgram(shader_program);

    // 查看结果
    int success;
    char log_info[LOG_INFO_LEN];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, LOG_INFO_LEN, nullptr, log_info);
        SPDLOG_ERROR("link shader fail, info log: {}", log_info);
        throw std::exception();
    }

    return shader_program;
}

void ShaderExtLight::set(Shader &shader, const SpotLight &light, const std::string &name) {
    shader.uniform_vec3_set(name + ".position", light.position);
    shader.uniform_vec3_set(name + ".direction", light.direction);

    set_light(shader, light, name);

    set_attenuation(shader, light.attenuation, name);

    // 设置切光角
    shader.uniform_float_set(name + ".inner_cutoff", light.inner_cutoff);
    shader.uniform_float_set(name + ".outer_cutoff", light.outer_cutoff);
}

void ShaderExtLight::set(Shader &shader, const DirLight &light, const std::string &name) {
    shader.uniform_vec3_set(name + ".direction", light.direction);

    set_light(shader, light, name);
}

void ShaderExtLight::set(Shader &shader, const PointLight &light, const std::string &name) {
    shader.uniform_vec3_set(name + ".position", light.position);

    set_attenuation(shader, light.attenuation, name);

    set_light(shader, light, name);
}

void ShaderExtLight::set_attenuation(Shader &shader, const AttenuationDistance &attenuation, const std::string &name) {
    shader.uniform_float_set(name + ".constant", attenuation.constant);
    shader.uniform_float_set(name + ".linear", attenuation.linear);
    shader.uniform_float_set(name + ".quadratic", attenuation.quadratic);
}

void ShaderExtLight::set_light(Shader &shader, const Light &light, const std::string &name) {
    shader.uniform_vec3_set(name + ".color.ambient", light.ambient);
    shader.uniform_vec3_set(name + ".color.diffuse", light.diffuse);
    shader.uniform_vec3_set(name + ".color.specular", light.specular);
}

void ShaderExtMVP::set(Shader &shader, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj) {
    shader.uniform_mat4_set("model", model);
    shader.uniform_mat4_set("view", view);
    shader.uniform_mat4_set("projection", proj);
}
