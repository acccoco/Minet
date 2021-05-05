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

#include "../config.hpp"


enum TextureType {
    diffuse,
    specular,
    normal,
};


class Texture2D {
public:
    friend class TextureManager;

    unsigned int id = 0;

    Texture2D() = default;

    explicit Texture2D(std::string path);

    /* 从文件中载入 hdr 材质 */
    static unsigned int hdr_load(const std::string &path);

    /* 创建立方体贴图，内容为空，大小为512x512，内部存储格式为 RGB16F */
    static unsigned int cubemap_texture_create(unsigned int width);

    static GLuint cubemap_tex_create(const std::vector<std::string> &files);

private:
    std::string path;
    int width = 0;
    int height = 0;
    int nr_channels = 0;

    /* 从文件中取得数据，传送到 GPU 中 */
    void init();

    /* 将材质输送到 gpu */
    static unsigned int regist_texture(unsigned char *data, int width, int height, int nr_channels);

    /* 加载图片 */
    static unsigned char *load_file(const std::string &file_path, int *_width, int *_height, int *_nr_channels);

    /* 创建一个 hdr 材质 */
    static unsigned int hdr_texture_create(int width, int height, float *data);
};


/* 多个 mesh 使用同一个 texture，这个类可以缓存 */
class TextureManager {
public:
    static std::shared_ptr<Texture2D> texture_load(const std::string &path);

private:
    static std::map<std::string, std::shared_ptr<Texture2D>> textures;
};


class Texture2DBuilder {
public:
    /* 去除 mesh 中的所有纹理，并构建 Texture 对象 */
    static std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>>
    build(const std::string &dir, const aiMesh &mesh, const aiScene &scene);

private:
    /* 获得 mesh 中特定类型的所有 texture */
    static std::vector<std::shared_ptr<Texture2D>>
    build(const std::string &dir, const aiMaterial &mat, aiTextureType type);
};

#endif //RENDER_TEXTURE_H
