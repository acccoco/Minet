#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <exception>

#include <glad/glad.h>
#include <easylogging++.h>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"
#include "light.h"


class Shader {
public:
    GLint id = 0;

    Shader(const std::string &vertex, const std::string &fragment);

    void uniform_vec4_set(const std::string &name, const glm::vec4 &v);

    void uniform_float_set(const std::string &name, GLfloat value);

    void uniform_vec3_set(const std::string &name, const glm::vec3 &v);

    void uniform_mat4_set(const std::string &name, const glm::mat4 &m);

    void uniform_tex2d_set(const std::string &name, GLuint texture_unit);

    void use() const;

    static void unuse();


protected:

    /**
     * 缓存：着色器中 uniform 变量的名称 和  location
     * @example
     * { "name": location, }
     */
    std::map<std::string, GLint> uniform_location_map;

    static GLuint shader_prog_get(const std::string &vert_shader_file,
                                  const std::string &frag_shader_file);

    static GLuint shader_compile(const std::string &file_name, GLenum shader_type);

    GLint unifrom_location_get(const std::string &name);
};

#endif //RENDER_SHADER_H
