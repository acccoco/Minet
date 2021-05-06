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


/* 面朝 +z 的二维正方形 */
const std::vector<float> plane_pt_2 = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
};


/* 面朝 +z 的三维正方形 */
const std::vector<float> plane_pt_3 = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f, 0.0f
};

#endif //RENDER_PLANE_H
