#ifndef RENDER_TEXTURE_BOX_H
#define RENDER_TEXTURE_BOX_H

#include "../engine/model.h"
#include "../engine/texture.h"
#include "../engine/shader.h"
#include "../engine/vao.h"
#include "cube.h"

class TextureBox: public Model {
private:
    Texture2D texture;
public:
    explicit TextureBox(const std::string& texture_file) {
        // 指定着色器，模型
        this->bind_shader(std::make_shared<Shader>(SHADER("tex.vert"), SHADER("tex.frag")));
        Cube cube;
        this->bind_mesh(std::make_shared<VAO>(cube.indices, cube.pos, cube.normal, cube.uv));

        // 设置纹理
        this->texture = Texture2D(texture_file);
        this->shader->uniform_tex2d_set("texture1", this->texture.texture_id);
    }

    void update() override {

    }

};

#endif //RENDER_TEXTURE_BOX_H
