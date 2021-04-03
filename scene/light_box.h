#ifndef RENDER_LIGHT_BOX_H
#define RENDER_LIGHT_BOX_H

#include "../engine/model.h"


class LightBox : public Model2 {
public:
    LightBox() {
        // 指定模型
        Cube cube;
        bind_mesh(std::make_shared<VAO>(cube.indices, cube.pos, cube.normal, cube.uv));

    }

    void update() override {}

};

#endif //RENDER_LIGHT_BOX_H
