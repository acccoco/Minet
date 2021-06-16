/**
 * 在这里定义一下着色器的变量名称，实现 c++ 代码和 shader 代码的统一
 *
 */

#ifndef RENDER_GLOBAL_H
#define RENDER_GLOBAL_H

/* 顶点属性的编号 */
struct VertAttribLocation {
    inline static const GLuint position = 0;
    inline static const GLuint normal = 1;
    inline static const GLuint texcoord = 2;
};

#endif //RENDER_GLOBAL_H
