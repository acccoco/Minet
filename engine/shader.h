#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <exception>
#include <functional>

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "model.h"
#include "global.h"
#include "texture.h"
#include "utils/with.h"


class Shader : public With {
public:
    GLuint id = 0;

    Shader(const std::string &vertex, const std::string &fragment, const std::vector<std::string> &macros = {},
           const std::string &geometry = "");


    // =====================================================
    // 设置 shader 的某个 uniform 变量
    // =====================================================

    inline void uniform_block(const std::string &name, GLuint index) const {
        glUseProgram(id);
        GLuint uniform_block_location = glGetUniformBlockIndex(id, name.c_str());
        glUniformBlockBinding(id, uniform_block_location, index);
    }

    inline void uniform_vec4_set(const std::string &name, const glm::vec4 &v) {
        glUseProgram(id);
        glUniform4f(_uniform_location_get(name), v.x, v.y, v.z, v.w);
    }

    inline void uniform_float_set(const std::string &name, GLfloat value) {
        glUseProgram(id);
        glUniform1f(_uniform_location_get(name), value);
    }

    inline void uniform_int_set(const std::string &name, GLint value) {
        glUseProgram(id);
        glUniform1i(_uniform_location_get(name), value);
    }

    inline void uniform_vec3_set(const std::string &name, const glm::vec3 &v) {
        glUseProgram(id);
        glUniform3f(_uniform_location_get(name), v.x, v.y, v.z);
    }

    inline void uniform_mat4_set(const std::string &name, const glm::mat4 &m) {
        glUseProgram(id);
        glUniformMatrix4fv(_uniform_location_get(name), 1, GL_FALSE, glm::value_ptr(m));
    }

    /**
     * 为 shader 的某个 texture sampler 指定纹理单元
     * @param texture_unit 应该是数字 0，1，2，...
     */
    inline void uniform_tex2d_set(const std::string &name, GLint texture_unit) {
        glUseProgram(id);
        glUniform1i(_uniform_location_get(name), texture_unit);
    }

    /**
     * 一次指定多个材质
     * @param texture_profile 多个材质的 id 以及在 shader 中的名称
     * @param start_unit 起始的纹理单元编号，默认为 0
     */
    void set_textures(const std::vector<std::tuple<std::string, GLuint>> &texture_profile,
                      GLsizei start_unit = 0);

    /**
     * 一次指定多个材质，自动从 mesh 中获取材质，如果 mesh 中没有，就跳过
     * @param start_unit 起始的纹理单元编号，默认为 0
     */
    void set_textures(const Mesh &mesh,
                      const std::vector<std::tuple<std::string, TextureType, unsigned>> &texture_profile,
                      GLsizei start_unit = 0);

    inline void use() const { glUseProgram(this->id); }

    inline void in() override { glUseProgram(this->id); }

    inline void out() override { glUseProgram(0); }

    // =====================================================
    // 绘制 Mesh 和 Model
    // =====================================================

    /* 设置绘制 Mesh 的方式 */
    inline void set_draw(const std::function<void(Shader &, const Mesh &)> &func) {
        this->_method_draw_mesh = func;
    }

    /**
     * 调用先前设定的绘制方式，绘制 Mesh；如果参数制定了绘制方式，这次绘制就使用参数指定的绘制方式
     */
    inline void draw(const Mesh &mesh, const std::function<void(Shader &, const Mesh &)> &func = nullptr) {
        glUseProgram(id);
        const auto &draw_func = (func == nullptr) ? _method_draw_mesh : func;
        draw_func(*this, mesh);
        mesh.draw();
    }

    /* 设置绘制 Model 的方式 */
    inline void set_draw(const std::function<void(Shader &, const Model &, const Mesh &)> &func) {
        this->_method_draw_model = func;
    }

    /**
     * 使用参数指定的绘制方式，绘制 Model
     * @param amount 在 instanced 绘制中需要用到
     */
    inline void draw(const Model &model,
                     const std::function<void(Shader &, const Model &, const Mesh &)> &func = nullptr,
                     GLsizei amount = 1) {

        glUseProgram(id);
        const auto &draw_func = (func == nullptr) ? _method_draw_model : func;
        for (const auto &mesh : model.meshes()) {
            draw_func(*this, model, mesh);
            mesh.draw(amount);
        }
    }

    // todo 每帧更新和每mesh 更新，可以再成体系一点，从命名开始

    // =====================================================
    // 数据绑定，用于每一帧时更新 shader
    // =====================================================

    inline void set_update_per_frame(const std::function<void(Shader &)> &func) {
        _method_update_per_frame = func;
    }

    /* 每一帧进行一次的更新 */
    inline void update_per_frame() {
        glUseProgram(id);
        _method_update_per_frame(*this);
    }

protected:
    /* 链接着色器程序 */
    static GLuint _shader_link(GLuint vertex, GLuint fragment, GLuint geometry = 0);

    /**
     * 编译着色器程序
     * @param file_name 存放 shader 代码的文件
     * @param shader_type shader 的类型，可以是 vertex，fragment，geometry
     * @param macros 需要注入的宏定义
     */
    static GLuint
    _shader_compile(const std::string &file_name, GLenum shader_type, const std::vector<std::string> &macros);

    /* 获得 shader 中 uniform 变量对应的 location */
    GLint _uniform_location_get(const std::string &name);

protected:
    /* 绘制 mesh 的方式 */
    std::function<void(Shader &, const Mesh &)> _method_draw_mesh
            = [](Shader &, const Mesh &) {};

    /* 绘制 model 的方式 */
    std::function<void(Shader &, const Model &, const Mesh &)> _method_draw_model
            = [](Shader &, const Model &, const Mesh &) {};

    // todo 是否应该追加一个参数：scene？
    /* 每一帧发生的更新 */
    std::function<void(Shader &)> _method_update_per_frame
            = [](Shader &) {};

    /**
     * 储存了着色器中 uniform 变量 name 和 location 的对应关系
     * @example
     * { "name": location, }
     */
    std::map<std::string, GLint> _uniform_location_map;
};


/*
 * 具有模版参数的 Shader，可以更加灵活地进行数据绑定
 * 主要用于 mesh 对象具有其他的属性的情况.
 * 比如 mesh 是一个立方体光源，那么它还具有 light_color 属性，就可以通过这个类来传递进去
 * */
template<class T>
class ShaderT : public Shader {
public:
    ShaderT(const std::string &vertex, const std::string &fragment, const std::vector<std::string> &macros = {},
            const std::string &geometry = "")
            : Shader(vertex, fragment, macros, geometry) {}

    void set_drawT(const std::function<void(Shader &, const Mesh &, const T &)> &func) {
        _template_method_draw_mesh = func;
    }

    void set_drawT(const std::function<void(Shader &, const Model &, const Mesh &, const T &)> &func) {
        _template_method_draw_model = func;
    }

    void draw_t(const Mesh &mesh, const T &t) {
        glUseProgram(id);
        _template_method_draw_mesh(*this, mesh, t);
        mesh.draw();
    }

    void draw_t(const Model &model, const T &t) {
        glUseProgram(id);
        for (const auto &mesh : model.meshes()) {
            _template_method_draw_model(*this, model, mesh, t);
            mesh.draw();
        }
    }

private:
    std::function<void(Shader &, const Mesh &, const T &)> _template_method_draw_mesh;
    std::function<void(Shader &, const Model &, const Mesh &, const T &)> _template_method_draw_model;
};

#endif //RENDER_SHADER_H
