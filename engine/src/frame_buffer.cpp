#include <exception>
#include <spdlog/spdlog.h>
#include "frame_buffer.h"


FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
        : width(width), height(height) {
    // 创建帧缓冲对象
    glGenFramebuffers(1, &this->frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);

    // 创建颜色附件
    glGenTextures(1, &this->color_buffer);
    glBindTexture(GL_TEXTURE_2D, this->color_buffer);
    /**
     * 参数说明
     * target：texture target
     * level：mipmap 的级别，0 是基础级别
     * interlnalFormat：颜色组件的数量
     * width, height：宽高
     * border：由于历史原因，这个值必须是 0
     * format：像素数据的格式
     * type：像素的数据类型
     * data：内存中图形数据的指针
     */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // uv坐标超出范围后如何采样：重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 设置帧缓冲对象的 0 号位颜色缓冲，使用 texture2D 填充，mipmap 级别设置为 0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->color_buffer, 0);

    // 创建渲染缓冲对象
    glGenRenderbuffers(1, &this->depth_stencil_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // 将渲染缓冲对象绑定到帧缓冲对象，作为深度缓冲附件和模板缓冲附件
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depth_stencil_buffer);

    // 检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("frame buffer is not complete.");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw std::exception();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::color_tex_get() const {
    return this->color_buffer;
}

void FrameBuffer::in() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);
}

void FrameBuffer::out() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DepthBuffer::DepthBuffer(GLuint width, GLuint height) {
    glGenFramebuffers(1, &frame_buffer);
    glGenTextures(1, &depth_buffer);

    // 生成 depth texture
    glBindTexture(GL_TEXTURE_2D, depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 为帧缓冲绑定颜色、深度附件
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, depth_buffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthFrameBuffer::DepthFrameBuffer(GLuint width, GLuint height) {
    glGenFramebuffers(1, &frame_buffer_id);
    glGenRenderbuffers(1, &render_buffer_id);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_id);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthFrameBuffer::in() {
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
}

void DepthFrameBuffer::out() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
