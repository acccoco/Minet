//
// Created by bigso on 2021/5/5.
//

#ifndef RENDER_PLANE_H
#define RENDER_PLANE_H

#include <vector>

// 面朝 +y 的正方形
const std::vector<float> plane_pnt = {
        5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        -5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
        -5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        5.0f, 0.f, 5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
        5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 2.0f, 2.0f,
        -5.0f, 0.f, -5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
};

#endif //RENDER_PLANE_H
