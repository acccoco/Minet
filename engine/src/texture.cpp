#include "../texture.h"

Texture2D::Texture2D(std::string path) : path(std::move(path)) {
    this->init();
}

void Texture2D::init() {
    // 加载图片
    unsigned char *data = this->load_file(this->path);

    // 输送到 GPU
    this->id = Texture2D::regist_texture(data, width, height, this->nr_channels);

    // 释放内存
    stbi_image_free(data);
}

unsigned int Texture2D::regist_texture(unsigned char *data, int width, int height, int nr_channels) {
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
    if (nr_channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (nr_channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else {
        SPDLOG_ERROR("invalid texture channel number: {}", nr_channels);
        throw std::exception();
    }

    // 生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);

    // 解绑
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

unsigned char *Texture2D::load_file(std::string &file_path) {
    SPDLOG_INFO("load texture from file: {}", file_path);
    unsigned char *data = stbi_load(file_path.c_str(), &this->width, &this->height, &this->nr_channels, 0);
    if (!data) {
        SPDLOG_ERROR("fail to load texture from file: {}", file_path);
        throw (std::exception());
    }
    return data;
}

unsigned int Texture2D::hdr_load(const std::string &path) {
    int width, height, nr_components;
    unsigned int hdr_texture;

    // 载入文件
    stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(path.c_str(), &width, &height, &nr_components, 0);
    if (!data) {
        SPDLOG_ERROR("fail to load hdr texture from file: {}", path);
        throw std::exception();
    }

    // 创建材质对象
    hdr_texture = hdr_texture_create(width, height, data);

    stbi_image_free(data);
    return hdr_texture;
}

unsigned int Texture2D::hdr_texture_create(int width, int height, float *data) {
    unsigned int hdr_texture;

    glGenTextures(1, &hdr_texture);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return hdr_texture;
}

unsigned int Texture2D::cubemap_texture_create(unsigned int width) {
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

    return cube_map;
}

std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>>
Texture2DBuilder::build(const std::string &dir, const aiMesh &mesh, const aiScene &scene) {
    std::map<TextureType, std::vector<std::shared_ptr<Texture2D>>> textures;

    // 先判断是否有材质
    if (mesh.mMaterialIndex < 0) {
        SPDLOG_INFO("this mesh has no material.");
        return textures;
    }

    aiMaterial *material = scene.mMaterials[mesh.mMaterialIndex];

    // diffuse
    textures.emplace(diffuse, build(dir, *material, aiTextureType_DIFFUSE));

    // specular
    textures.emplace(specular, build(dir, *material, aiTextureType_SPECULAR));

    // normal
    // textures.emplace(normal, build(dir, *material, aiTextureType_NORMALS));

    return textures;
}

std::vector<std::shared_ptr<Texture2D>>
Texture2DBuilder::build(const std::string &dir, const aiMaterial &mat, aiTextureType type) {
    std::vector<std::shared_ptr<Texture2D>> textures;

    for (unsigned int i = 0; i < mat.GetTextureCount(type); ++i) {
        // 取得 texture 的全路径
        aiString file_name;
        mat.GetTexture(type, i, &file_name);
        std::string full_path = dir + file_name.C_Str();

        // 构造 textures
        auto texture = TextureManager::texture_load(full_path);
        textures.push_back(texture);
    }

    return textures;
}

std::map<std::string, std::shared_ptr<Texture2D>> TextureManager::textures;

std::shared_ptr<Texture2D> TextureManager::texture_load(const std::string &path) {
    auto iter = textures.find(path);

    // 使用缓存
    if (iter != textures.end())
        return iter->second;

    auto texture = std::make_shared<Texture2D>(path);
    textures.emplace(path, texture);
    return texture;
}
