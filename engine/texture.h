/**
 * 材质对象
 */

#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include <string>
#include <exception>
#include <utility>

#include <glad/glad.h>
#include <stb_image.h>
#include <easylogging++.h>

#include "../config.hpp"

class Texture2D {
public:
    unsigned int id = 0;
    std::string type;

    Texture2D() = default;

    explicit Texture2D(std::string path, std::string type)
            : type(std::move(type)), path(std::move(path)) {
        this->set_up();
    }

private:

    std::string path;
    int width = 0;
    int height = 0;
    int nr_channels = 0;

    void set_up() {
        // 加载图片
        unsigned char *data = this->load_file(this->path);

        // 输送到 GPU
        this->id = Texture2D::regist_texture(data, width, height);

        // 释放内存
        stbi_image_free(data);
    }

    // 将材质输送到 gpu
    static unsigned int regist_texture(unsigned char *data, int width, int height) {
        glUseProgram(0);        // 任何着色器都不会受影响

        unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // uv坐标超出范围后如何采样：重复
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // 材质放大和缩小时，应该如何采样
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 将纹理发送到显存
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // 生成多级渐远纹理
        glGenerateMipmap(GL_TEXTURE_2D);

        // 解绑
        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    // 加载图片
    unsigned char *load_file(std::string &file_path) {
        LOG(INFO) << "load texture from file: " << file_path;
        unsigned char *data = stbi_load(file_path.c_str(), &this->width, &this->height, &this->nr_channels, 0);
        if (!data) {
            LOG(ERROR) << "fail to load texture from file: " << file_path;
            throw (std::exception());
        }
        return data;
    }

};

#endif //RENDER_TEXTURE_H
