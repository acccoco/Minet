#include "texture.h"


Texture2D::Texture2D(const std::string &path, TextureWrap wrap, TextureColorFormat color_format, bool mip_map,
                     bool flip) {
    // 载入文件
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (!data) {
        throw std::runtime_error(fmt::format("fail to load texture file: {}", path));
    }

    /* 生成 texture */
    glGenTextures(1, &_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _id);

    /* 传输纹理数据 */
    if (color_format == TextureColorFormat::Auto) {
        switch (nr_channels) {
            case 1:
                color_format = TextureColorFormat::RED;
                break;
            case 3:
                color_format = TextureColorFormat::RGB;
                break;
            case 4:
                color_format = TextureColorFormat::RGBA;
                break;
            default:
                throw std::runtime_error(fmt::format("bad nr_channels: {}", nr_channels));
        }
    }
    switch (color_format) {
        case TextureColorFormat::RED:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            break;
        case TextureColorFormat::RGB:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case TextureColorFormat::RGBA:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        default:
            throw std::runtime_error("bad texture color format.");
    }

    /* 超出范围后如何采样 */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));

    /* 缩放后如何采样 */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 生成 mipmap */
    if (mip_map) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    /* 解除绑定 */
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}

std::shared_ptr<Texture2D> TextureManager::texture_load(const std::string &path) {
    auto iter = _textures.find(path);

    // 使用缓存
    if (iter != _textures.end())
        return iter->second;

    auto texture = std::make_shared<Texture2D>(path);
    _textures.emplace(path, texture);
    return texture;
}


std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>>
TextureManager::textures_get(const aiMaterial& material, const std::string &dir) {

    /* Assimp 和 自定义材质类型的对应表 */
    static std::map<TextureType, aiTextureType> type_map {
            {TextureType::diffuse, aiTextureType_DIFFUSE},
            {TextureType::specular, aiTextureType_SPECULAR},
            {TextureType::normal, aiTextureType_NORMALS},
    };

    /* 需要从文件中提取出的 texture 类型 */
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures {
            {TextureType::diffuse, {}},
            {TextureType::specular, {}},
    };

    for (auto &[tex_type, texs] : textures) {
        aiTextureType ai_tex_type = type_map[tex_type];
        aiString file_name;
        std::string full_path;
        for (unsigned i = 0; i < material.GetTextureCount(ai_tex_type); ++i) {
            material.GetTexture(ai_tex_type, i, &file_name);
            full_path = dir + file_name.C_Str();
            texs.push_back(TextureManager::texture_load(full_path));
        }
    }

    return textures;
}

GLuint TextureCube::cube_map_create(GLsizei width) {
    unsigned int cube_map;

    glGenTextures(1, &cube_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);
    /* 顺序依次是：+x, -x, +y, -y, +z, -z */
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, width, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return cube_map;
}

TextureCube::TextureCube(const std::string &file_path_positive_x, const std::string &file_path_negative_x,
                         const std::string &file_path_positive_y, const std::string &file_path_negative_y,
                         const std::string &file_path_positive_z, const std::string &file_path_negative_z) {
    std::map<GLenum, std::string> texture_map{
            {GL_TEXTURE_CUBE_MAP_POSITIVE_X, file_path_positive_x},
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, file_path_negative_x},
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, file_path_positive_y},
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, file_path_negative_y},
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, file_path_positive_z},
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, file_path_negative_z},
    };

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

    /* 读取文件，生成纹理 */
    int width, height, nr_channels;
    unsigned char *data;
    for (const auto&[texure_target, file_path] : texture_map) {
        data = stbi_load(file_path.c_str(), &width, &height, &nr_channels, 0);
        if (!data) {
            throw std::runtime_error(fmt::format("fail to load texture: {}", file_path));
        }
        switch (nr_channels) {
            case 3:
                glTexImage2D(texure_target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                break;
            case 4:
                glTexImage2D(texure_target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            default:
                throw std::runtime_error("bad nr channels");
        }
        stbi_image_free(data);
    }

    /* 多级纹理 */
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* uv 超过后如何采样 */
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TextureHDR::TextureHDR(const std::string &file_path) {
    // note 这里进行了垂直翻转
    stbi_set_flip_vertically_on_load(true);
    int width, height, nr_channels;
    // 载入文件
    float *data = stbi_loadf(file_path.c_str(), &width, &height, &nr_channels, 0);
    if (!data) {
        throw std::runtime_error(fmt::format("fail to load hdr texture from file: {}", file_path));
    }
    stbi_set_flip_vertically_on_load(false);

    // 创建材质对象
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    /* 将像素写入纹理 */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

    /* 超过 tex_coord 范围后，如何采样 */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* 生成 */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 关闭文件
    stbi_image_free(data);
}