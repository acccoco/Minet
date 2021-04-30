#include "../frame_buffer.h"


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
