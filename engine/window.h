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
};


class Window {
public:
    // 鼠标的坐标
    static double mouse_x, mouse_y;
    static bool mouse_init;            // 鼠标是否开始移动了

    // 键盘事件
    static std::vector<KeyboardEvent> key_events;

    // 存放键盘和鼠标的回调函数
    static DelegateList<double, double> mouse_pos_delegates;
    static DelegateList<const std::vector<KeyboardEvent>&> keyboard_delegates;

    static GLFWwindow *window;

    /* 获得全局的 window 对象 */
    static Window* get();

    /* 初始化全局的对象 */
    static void init();

    /* 将窗口对象设置为上下文对象 */
    static void use() ;

    /**
     * 在每个渲染循环中使用
     * @作用 检测键盘事件
     */
    static void update() ;

    /* 是否需要关闭窗口 */
    static bool need_close() ;

private:
    // 窗口的一些参数
    const int width = 1600;
    const int height = 1200;
    const std::string title = "AccRender";

    /* 创建窗口，注册回调函数，捕捉光标等 */
    Window() noexcept;

    /* 鼠标位置的回调，实际是调用委托列表 */
    static void mouse_pos_cbk(GLFWwindow *window, double x, double y);

    /* 窗口大小改变的回调：同步改变 OpenGL 的视口大小 */
    static void frame_buffer_cbk(GLFWwindow *_window, int _width, int _height);

    /* 窗口关闭的键盘回调 */
    static void close_window_cbk(const std::vector<KeyboardEvent> &events);
};

#endif //RENDER_WINDOW_H
