#ifndef RENDER_FRAMEBUFFER_H
#define RENDER_FRAMEBUFFER_H


#include <glad/glad.h>

/**
 * 帧缓冲，规格如下：
 *  颜色缓冲0：纹理附件
 *  深度-模板缓冲：渲染缓冲对象附件
 */
class FrameBuffer {
public:

    /* 创建帧缓冲对象及附件 */
    FrameBuffer(unsigned int width, unsigned int height);

    void use() const;

    /* 获取颜色缓冲的纹理 */
    GLuint color_tex_get() const;

private:
    // 帧缓冲的 id
    GLuint frame_buffer{};
    // 颜色缓冲的 id，这里颜色缓冲用的是 texture2D
    GLuint color_buffer{};
    // 渲染缓冲对象的 id，用作深度缓冲和模板缓冲
    GLuint depth_stencil_buffer{};

    unsigned int width, height;
};

#endif //RENDER_FRAMEBUFFER_H
