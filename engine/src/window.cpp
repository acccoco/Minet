#include "../window.h"


DelegateList<double, double> Window::mouse_pos_delegates;
DelegateList<const std::vector<KeyboardEvent> &> Window::keyboard_delegates;

GLFWwindow *Window::window = nullptr;


void Window::use() {
    glfwMakeContextCurrent(Window::window);
}

void Window::mouse_pos_cbk(GLFWwindow *_window, double x, double y) {
    mouse_pos_delegates.invoke(x, y);
}

bool Window::need_close() {
    return glfwWindowShouldClose(Window::window);
}

void Window::update() {
    // 键盘按键对应的事件表
    static std::map<int, KeyboardEvent> keyboard_event_map{
            {GLFW_KEY_W,      KeyboardEvent::press_W},
            {GLFW_KEY_A,      KeyboardEvent::press_A},
            {GLFW_KEY_S,      KeyboardEvent::press_S},
            {GLFW_KEY_D,      KeyboardEvent::press_D},
            {GLFW_KEY_Q,      KeyboardEvent::press_Q},
            {GLFW_KEY_E,      KeyboardEvent::press_E},
            {GLFW_KEY_ESCAPE, KeyboardEvent::press_Esc},
    };

    // 查表，将 GLFW 的键盘封装为自定义的事件类型
    std::vector<KeyboardEvent> events;
    for (const auto &iter : keyboard_event_map) {
        if (glfwGetKey(window, iter.first) == GLFW_PRESS)
            events.push_back(iter.second);
    }

    // 调用注册键盘事件的一系列委托
    keyboard_delegates.invoke(events);
}

void Window::frame_buffer_cbk(GLFWwindow *_window, int _width, int _height) {
    glViewport(0, 0, _width, _height);
}

Window::Window() {
    assert(width > 0 && height > 0);
    SPDLOG_INFO("create window ({:d}, {:d})", width, height);

    // 创建窗口
    Window::window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        SPDLOG_ERROR("fail to create window.");
        glfwTerminate();
        exit(-1);
    }

    // 初始化
    this->init();
}

void Window::close_window_cbk(const std::vector<KeyboardEvent> &events) {
    if (std::find(events.begin(), events.end(), KeyboardEvent::press_Esc) != events.end()) {
        SPDLOG_INFO("check ESC is pressed, window will be closed");
        glfwSetWindowShouldClose(window, true);
    }
}

void Window::init() {
    // 注册回调函数
    glfwSetCursorPosCallback(window, this->mouse_pos_cbk);
    glfwSetFramebufferSizeCallback(window, frame_buffer_cbk);

    // 关闭窗口的回调
    keyboard_delegates.add(new FuncDelegate<const std::vector<KeyboardEvent> &>(close_window_cbk));

    // 捕捉光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
