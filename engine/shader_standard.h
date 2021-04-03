/**
 * 着色器中变量的规范
 */
#ifndef RENDER_SHADER_STANDARD_H
#define RENDER_SHADER_STANDARD_H

#include <string>

#include <glad/glad.h>


struct ELocation {
    static const GLuint position;
    static const GLuint normal;
    static const GLuint texcoord;
};


struct EMatrix {
    static const std::string model;
    static const std::string view;
    static const std::string projection;
};


struct ETextureType {
    static const std::string diffuse;
    static const std::string specular;
    static const std::string normal;

    inline static std::string diffuse_name(GLuint index) {
        return name_get(diffuse, index);
    }
    inline static std::string specular_name(GLuint index) {
        return name_get(specular, index);
    }
    inline static std::string normal_name(GLuint index) {
        return name_get(normal, index);
    }

private:
    inline static std::string name_get(const std::string &type, GLuint index) {
        // "material.texture_diffuse_0"
        return "material.texture_" + type + "_" + std::to_string(index);
    }
};

#endif //RENDER_SHADER_STANDARD_H
