
#include <cassert>
#include <exception>

#include <fmt/format.h>

#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "utils/file.h"


// 全局变量 =======================================================================
const int LOG_INFO_LEN = 512;


// 类方法实现 ======================================================================
GLint Shader::_uniform_location_get(const std::string &name) {
    auto iter = _uniform_location_map.find(name);

    // 没找到，需要调用 OpenGL 的接口查询
    if (iter == _uniform_location_map.end()) {
        int location = glGetUniformLocation(this->id, name.c_str());
        if (location == -1) {
            SPDLOG_ERROR("fail to find shader uniform: {}", name);
            throw (std::exception());
        }
        _uniform_location_map.insert({name, location});
        return location;
    }

    // 找到了
    return iter->second;
}


Shader::Shader(const std::string &vertex, const std::string &fragment, const std::vector<std::string> &macros,
               const std::string &geometry) {
    GLuint id_vertex;
    GLuint id_fragment;
    GLuint id_geometry;

    /* 编译着色器 */
    id_vertex = _shader_compile(vertex, GL_VERTEX_SHADER, macros);
    id_fragment = _shader_compile(fragment, GL_FRAGMENT_SHADER, macros);
    id_geometry = geometry.empty() ? 0 : _shader_compile(geometry, GL_GEOMETRY_SHADER, macros);

    /* 链接着色器 */
    this->id = _shader_link(id_vertex, id_fragment, id_geometry);

    /* 删除着色器对象 */
    glDeleteShader(id_vertex);
    glDeleteShader(id_fragment);
}


GLuint
Shader::_shader_compile(const std::string &file_name, GLenum shader_type, const std::vector<std::string> &macros) {
    assert(shader_type == GL_VERTEX_SHADER
           || shader_type == GL_FRAGMENT_SHADER
           || shader_type == GL_GEOMETRY_SHADER);

    /* 逐行读取文件，在版本声明后追加宏定义 */
    std::vector<std::string> lines = File::file_load_lines(file_name);
    std::string shader_source;
    for (const auto &line : lines) {
        shader_source += line;

        /* 在 #version 后面追加宏定义 */
        if (line.find("#version") != std::string::npos) {
            for (auto const &macro : macros) {
                shader_source += fmt::format("#define {}\n", macro);
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


GLuint Shader::_shader_link(GLuint vertex, GLuint fragment, GLuint geometry) {
    // 链接着色器
    SPDLOG_INFO("link shader");
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    if (geometry != 0)
        glAttachShader(shader_program, geometry);
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

void Shader::set_textures(const std::vector<std::tuple<std::string, GLuint>> &texture_profile, GLsizei start_unit) {
    glUseProgram(id);
    assert(start_unit >= 0);
    GLsizei texture_unit = start_unit;
    for (auto &[texture_name, texture_id] : texture_profile) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(_uniform_location_get(texture_name), texture_unit);
        texture_unit++;
    }
}

void Shader::set_textures(const Mesh &mesh,
                          const std::vector<std::tuple<std::string, TextureType, unsigned int>> &texture_profile,
                          GLsizei start_unit) {

    glUseProgram(id);
    assert(start_unit >= 0);
    GLsizei texture_unit = start_unit;

    for (auto &[texture_name, texture_type, idx]: texture_profile) {
        /* 如果 mesh 的特定类型特定序号的材质不存在，就跳过 */
        auto textures = mesh.textures(texture_type);
        if (idx >= textures.size())
            continue;

        /* 为 shader 绑定材质 */
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, textures[idx]->id());
        glUniform1i(_uniform_location_get(texture_name), texture_unit);
        texture_unit++;
    }
}
