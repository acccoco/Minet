#ifndef RENDER_ENGINE_WINDOW_H
#define RENDER_ENGINE_WINDOW_H

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cassert>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "./delegate.h"


/* 键盘按下的事件 */
enum class KeyboardEvent {
    press_W,
    press_A,
    press_S,
    press_D,
    press_Q,
    press_E,
    press_Esc,
    press_LAlt,
};


/* 鼠标按键按下的事件 */
enum class MouseButtonEvent {
    press_Left,
    press_Right,
};


class Window {
public:
    // 鼠标的坐标
    static double mouse_x, mouse_y;
    // 鼠标是否开始移动了
    static bool mouse_init;
    // 键盘事件
    static std::vector<KeyboardEvent> key_events;
    // 鼠标按键按下
    static std::vector<MouseButtonEvent> mouse_button_events;

    static GLFWwindow *window;

    /* 初始化全局的对象 */
    static void init();

    /* 将窗口对象设置为上下文对象 */
    static void use();

    /* 在每个渲染循环中使用 */
    static void update();

    /* 是否需要关闭窗口 */
    static bool need_close();

private:
    // 窗口的一些参数
    const int width = 1600;
    const int height = 1200;
    const std::string title = "AccRender";

    /* 创建窗口，注册回调函数，捕捉光标等 */
    Window() noexcept;

    /* 鼠标位置的回调，实际是调用委托列表 */
    static void mouse_pos_cbk(GLFWwindow *_window, double x, double y);

    /* 鼠标按键按下的回调 */
    static void mouse_button_cbk(GLFWwindow *_window, int button, int action, int mods);

    /* 窗口大小改变的回调：同步改变 OpenGL 的视口大小 */
    static void frame_buffer_cbk(GLFWwindow *_window, int _width, int _height);

    /* 检测键盘事件 */
    static void keyboard_events_check();

    /* 检测是否需要关闭窗口 */
    static void close_window_check();
};

#endif //RENDER_WINDOW_H
