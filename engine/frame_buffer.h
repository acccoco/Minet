#ifndef RENDER_FRAME_BUFFER_H
#define RENDER_FRAME_BUFFER_H

#include <glad/glad.h>

/* 用于阴影绘制的深度缓冲 */
class DepthBuffer {
public:
    DepthBuffer(GLuint width, GLuint height);

private:
    GLuint frame_buffer{};
    GLuint depth_buffer{};
};


#endif //RENDER_FRAME_BUFFER_H
