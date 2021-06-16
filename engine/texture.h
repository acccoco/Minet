#ifndef RENDER_ENGINE_TEXTURE_H
#define RENDER_ENGINE_TEXTURE_H

#include <map>
#include <string>
#include <exception>
#include <utility>

#include <glad/glad.h>
#include <stb_image.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>


/* 纹理的类型 */
enum TextureType {
    diffuse,
    specular,
    normal,
};

/* 超出范围后如何采样（这个类存在的意义：通过静态类型系统来保证参数有效） */
enum class TextureWrap {
    REPEAT = GL_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
};


/* 纹理的元素类型 */
enum class TextureColorFormat {
    Auto = 0,
    RED = GL_RED,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
};


class Texture2D {
public:
    friend class TextureManager;

    /**
     * 从文件中创建 texture 对象
     * @param path 文件的路径
     * @param wrap 超过 tex_coord 范围后，如何采样
     * @param color_format 颜色格式，比如是否有透明通道
     * @param mip_map 生成一系列缩放图
     * @param flip 在加载纹理是是否进行翻转
     */
    explicit Texture2D(const std::string &path,
                       TextureWrap wrap = TextureWrap::REPEAT,
                       TextureColorFormat color_format = TextureColorFormat::Auto,
                       bool mip_map = true, bool flip = false);

    [[nodiscard]] inline GLuint id() const { return _id; }

private:
    GLuint _id{0};

};


/**
 * HDR 贴图，使用了等距柱状投影
 * 纹理格式是浮点数！
 */
class TextureHDR {
public:
    explicit TextureHDR(const std::string &file_path);

    [[nodiscard]] inline GLuint id() const { return _id; }

private:
    GLuint _id{0};
};


/* 立方体贴图 */
class TextureCube {
public:
    TextureCube(const std::string &file_path_positive_x, const std::string &file_path_negative_x,
                const std::string &file_path_positive_y, const std::string &file_path_negative_y,
                const std::string &file_path_positive_z, const std::string &file_path_negative_z);

    /* 创建空的立方体贴图，每个面都是正方形，元素类型是 float */
    static GLuint cube_map_create(GLsizei width);

    [[nodiscard]] inline GLuint id() const { return _id; }

private:
    GLuint _id{0};
};


/* 多个 mesh 使用同一个 texture，这个类可以缓存 */
class TextureManager {
public:
    /* 从文件中读取纹理，会优先查看缓存 */
    static std::shared_ptr<Texture2D> texture_load(const std::string &path);

    /**
     * 使用 Assimp 载入 mesh 的所有 texture，也就是各种类型的 texture，比如 diffuse 和 normal
     * @param dir 模型文件的目录
     */
    static std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>>
    textures_get(const aiMaterial &material, const std::string &dir);

private:
    /* 文件名 - Texture 的表，用来缓存 texture 的 */
    inline static std::map<std::string, std::shared_ptr<Texture2D>> _textures;

};


#endif //RENDER_TEXTURE_H
