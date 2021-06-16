#include "window.h"


void Window::_mouse_pos_cbk(GLFWwindow *, double x, double y) {
    _mouse_cur_x = x;
    _mouse_cur_y = y;
}

void Window::_check_keyboard_event() {
    _key_events.clear();
    for (auto[glfw_key, key_event]: KEY_MAP) {
        if (glfwGetKey(_window, glfw_key) == GLFW_PRESS)
            _key_events.push_back(key_event);
    }
}

void Window::_close_window_check() {
    if (std::find(_key_events.begin(), _key_events.end(), KeyboardEvent::press_Esc) == _key_events.end())
        return;
    glfwSetWindowShouldClose(_window, true);
}

void Window::_mouse_buttion_cbk(GLFWwindow *, int button, int action, int mods) {

    /* 如果当前按键不在关注列表里面，就不管 */
    auto button_iter = MOUSE_BUTTON_MAP.find(button);
    if (button_iter == MOUSE_BUTTON_MAP.end())
        return;

    /* 如果在关注关心的鼠标按键里面，就添加相应的事件 */
    if (action == GLFW_PRESS) {
        _mouse_button_events[button_iter->second] = ButtonEvent::Press;
    } else if (action == GLFW_RELEASE) {
        _mouse_button_events[button_iter->second] = ButtonEvent::Release;
    }
}

void Window::_frame_buffer_size_cbk(GLFWwindow *, int width, int height) {
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);
}
