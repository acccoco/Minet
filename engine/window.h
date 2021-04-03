#ifndef RENDER_ENGINE_WINDOW_H
#define RENDER_ENGINE_WINDOW_H
#include <vector>
#include <GLFW/glfw3.h>
/**
 * 窗口注册相关的
 */
struct WindowCBK {
    typedef void (*MouseCbk)(GLFWwindow*, double, double);
    typedef void (*FrameBufferCbk)(GLFWwindow*, int, int);
    typedef void (*KeyboardCheck)(GLFWwindow*);

    MouseCbk mouse_cbk;
    FrameBufferCbk frame_buffer_cbk;
    std::vector<KeyboardCheck> keyboard_checks;
};


WindowCBK window_cbk;

#endif //RENDER_WINDOW_H
