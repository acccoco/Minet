#ifndef RENDER_FLOOR_H
#define RENDER_FLOOR_H

#include <vector>

// 面朝 +y 的正方形
const std::vector<float> floor_mesh = {
        5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        -5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
        -5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
        -5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
};

#endif //RENDER_FLOOR_H
