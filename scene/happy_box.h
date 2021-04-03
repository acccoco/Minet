#ifndef RENDER_HAPPY_BOX_H
#define RENDER_HAPPY_BOX_H

#include <string>
#include <memory>

#include "../engine/camera.h"
#include "../engine/model.h"
#include "../engine/texture.h"
#include "../engine/vao.h"
#include "../engine/shader.h"
#include "../engine/light.h"

#include "./cube.h"


class HappyBox : public Model2 {
private:
    Texture2D tex_diffuse;
    Texture2D tex_specular;

public:
    HappyBox() {
        // 指定模型
        Cube cube;
        bind_mesh(std::make_shared<VAO>(cube.indices, cube.pos, cube.normal, cube.uv));
    }


    void update() override {

    }

};

#endif //RENDER_HAPPY_BOX_H
