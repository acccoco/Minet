#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>


#include "./shader.h"
#include "./shader_standard.h"
#include "./texture.h"


/**
 * 顶点：坐标，法线，纹理坐标
 */
struct Vertex {

    friend class VertexBuilder;

    glm::vec3 positon;
    glm::vec3 normal;
    glm::vec2 texcoord = glm::vec2(0.f, 0.f);

    Vertex() = default;

};


// 面（顶点索引）
struct Face {
    unsigned int a{};
    unsigned int b{};
    unsigned int c{};

    unsigned int &x = a;
    unsigned int &y = b;
    unsigned int &z = c;

    Face() = default;
};


class Mesh {
public:

    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<std::shared_ptr<Texture2D>> textures;

    std::shared_ptr<Shader> shader;

    Mesh(std::vector<Vertex> vertices, std::vector<Face> &faces,
         std::vector<std::shared_ptr<Texture2D>> textures);

    void shader_bind(const std::shared_ptr<Shader> &_shader);

    void draw();

private:
    GLuint VAO{}, VBO{}, EBO{};

    struct TextureMap {
        std::string name;
        std::shared_ptr<Texture2D> texture;

        TextureMap() = default;

        TextureMap(const std::string &name, const std::shared_ptr<Texture2D> &texture) :
                name(name), texture(texture) {}
    };

    std::vector<TextureMap> texture_map;

    void init_texture();

    void set_up();
};

#endif //RENDER_MESH_H
