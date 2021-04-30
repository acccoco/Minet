#ifndef RENDER_FRAME_BUFFER_H
#define RENDER_FRAME_BUFFER_H

#include <glad/glad.h>
#include "./utils/with.h"

/* 用于阴影绘制的深度缓冲 */
class DepthBuffer {
public:
    DepthBuffer(GLuint width, GLuint height);

private:
    GLuint frame_buffer{};
    GLuint depth_buffer{};
};


/* 已经绑定了 depth 的 frame buffer */
class DepthFrameBuffer : public With {
public:
    DepthFrameBuffer(GLuint width, GLuint height);

    void in() override;

    void out() override;

private:
    GLuint frame_buffer_id{};
    GLuint render_buffer_id{};
};


#endif //RENDER_FRAME_BUFFER_H
