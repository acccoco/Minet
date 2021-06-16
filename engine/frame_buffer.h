#ifndef RENDER_FRAME_BUFFER_H
#define RENDER_FRAME_BUFFER_H

#include <glad/glad.h>
#include "utils/with.h"

/**
 * 帧缓冲，规格如下：
 *  颜色缓冲0：纹理附件
 *  深度-模板缓冲：渲染缓冲对象附件
 */
class FrameBuffer : public With{
public:

    /* 创建帧缓冲对象及附件 */
    FrameBuffer(unsigned int width, unsigned int height);

    /* 获取颜色缓冲的纹理 */
    [[nodiscard]] GLuint color_tex_get() const;

    void in() override;

    void out() override;

private:
    // 帧缓冲的 id
    GLuint frame_buffer{};
    // 颜色缓冲的 id，这里颜色缓冲用的是 texture2D
    GLuint color_buffer{};
    // 渲染缓冲对象的 id，用作深度缓冲和模板缓冲
    GLuint depth_stencil_buffer{};

    unsigned int width, height;
};


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
