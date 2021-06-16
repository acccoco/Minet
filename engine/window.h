#ifndef RENDER_ENGINE_WINDOW_H
#define RENDER_ENGINE_WINDOW_H

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cassert>

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>


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

/* 鼠标按键 */
enum class MouseButton {
    Left, Right,
};

/* 按键的动作类型 */
enum class ButtonEvent {
    Press, Release,
};


class Window {
public:

    /**
     * Window 是一个全局的对象，通过这个函数来初始化
     * @param title
     * @param height
     * @param w_aspect, h_aspect 长宽比，比如：16: 9
     */
    static void init(const std::string &title, int height, int w_aspect, int h_aspect) {

        /* 创建窗口 */
        assert(height > 0 && w_aspect > 0 && h_aspect > 0);
        _height = height;
        _width = height / h_aspect * w_aspect;
        _window = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
        if (_window == nullptr) {
            SPDLOG_ERROR("fail to create window.");
            glfwTerminate();
            exit(-1);
        }

        /* 设置窗口的固定长宽比 */
        glfwSetWindowAspectRatio(_window, w_aspect, h_aspect);

        /* 设置上下文为当前窗口 */
        glfwMakeContextCurrent(_window);

        /* 注册回调：鼠标位置改变 */
        glfwSetCursorPosCallback(_window, _mouse_pos_cbk);

        /* 注册回调：鼠标按键 */
        glfwSetMouseButtonCallback(_window, _mouse_buttion_cbk);

        /* 注册回调：窗口大小改变 */
        glfwSetFramebufferSizeCallback(_window, _frame_buffer_size_cbk);
    }

    /* 每一帧需要更新的内容 */
    static void update() {
        glfwPollEvents();

        /* 检查键盘按键 */
        _check_keyboard_event();

        /* 检测是否应该关闭当前窗口 */
        _close_window_check();

        /* 更新鼠标的位置。每一帧都计算，才能得到正确的 delta position */
        if (_mouse_last_x >= 0) {
            _mouse_delta_x = _mouse_cur_x - _mouse_last_x;
            _mouse_delta_y = _mouse_cur_y - _mouse_last_y;
        }
        _mouse_last_x = _mouse_cur_x;
        _mouse_last_y = _mouse_cur_y;
    }

    /* 销毁窗口对象 */
    inline static void destroy() {
        glfwDestroyWindow(_window);
    }

    inline static bool should_close() {
        return glfwWindowShouldClose(_window);
    }


    // =====================================================
    // 属性
    // =====================================================

    inline static GLFWwindow *window() { return _window; }

    inline static int width() { return _width; }

    inline static int height() { return _height; }

    inline static double mouse_x() { return _mouse_cur_x; }

    inline static double mouse_y() { return _mouse_cur_y; }

    inline static double mouse_last_x() { return _mouse_last_x; }

    inline static double mouse_last_y() { return _mouse_last_y; }

    inline static const std::vector<KeyboardEvent> &key_events() { return _key_events; }

    inline static const std::map<MouseButton, ButtonEvent> &mouse_button_events() { return _mouse_button_events; }

    /**
     * 鼠标自上一帧起移动了多远
     * @return (delta_x, delta_y)
     */
    inline static std::tuple<double, double> delta_mouse_pos() {
        return {_mouse_delta_x, _mouse_delta_y};
    }

private:
    /* 鼠标位置改变的回调函数 */
    static void _mouse_pos_cbk(GLFWwindow *, double x, double y);

    /* 检测键盘按键 */
    static void _check_keyboard_event();

    /* 检测按键，判断是否应该关闭窗口 */
    static void _close_window_check();

    /**
     * 鼠标按键按下的回调函数
     * @param button glfw 的鼠标按键
     * @param action 按键多做，press，release
     * @param mods
     */
    static void _mouse_buttion_cbk(GLFWwindow *, int button, int action, int mods);

    /* 窗口大小（framebuffer）改变的回调：同步改变 OpenGL 的 viewPort */
    static void _frame_buffer_size_cbk(GLFWwindow *, int width, int height);

    /* 不允许使用构造函数 */
    Window() = default;

private:
    inline static GLFWwindow *_window{nullptr};

    inline static int _width = 1200, _height = 900;

    /* 上一 frame 鼠标在 window 中的坐标，-1 表示无效值 */
    inline static double _mouse_last_x = -1, _mouse_last_y = -1;
    inline static double _mouse_cur_x = -1, _mouse_cur_y = -1;
    inline static double _mouse_delta_x = 0, _mouse_delta_y = 0;

    /* 比较关注哪些鼠标按键 */
    inline static const std::map<int, MouseButton> MOUSE_BUTTON_MAP{
            {GLFW_MOUSE_BUTTON_LEFT,  MouseButton::Left},
            {GLFW_MOUSE_BUTTON_RIGHT, MouseButton::Right},
    };

    /* 比较关注哪些按键 */
    inline static const std::map<int, KeyboardEvent> KEY_MAP{
            {GLFW_KEY_W,        KeyboardEvent::press_W},
            {GLFW_KEY_A,        KeyboardEvent::press_A},
            {GLFW_KEY_S,        KeyboardEvent::press_S},
            {GLFW_KEY_D,        KeyboardEvent::press_D},
            {GLFW_KEY_Q,        KeyboardEvent::press_Q},
            {GLFW_KEY_E,        KeyboardEvent::press_E},
            {GLFW_KEY_ESCAPE,   KeyboardEvent::press_Esc},
            {GLFW_KEY_LEFT_ALT, KeyboardEvent::press_LAlt},
    };

    /* 当前 frame 收集到的键盘事件 */
    inline static std::vector<KeyboardEvent> _key_events{};

    /* 当前 frame 收集到的鼠标按键事件 */
    inline static std::map<MouseButton, ButtonEvent> _mouse_button_events{};
};


#endif //RENDER_WINDOW_H
