#include "../window.h"
#include "../core.h"
#include "../utils/common.h"


Window *g_window = nullptr;
double Window::mouse_x;
double Window::mouse_y;
std::vector<KeyboardEvent> Window::key_events;
std::vector<MouseButtonEvent> Window::mouse_button_events;
bool Window::mouse_init = false;

int Window::width = 1600;
int Window::height = 1200;

GLFWwindow *Window::window = nullptr;


void Window::use() {
    glfwMakeContextCurrent(Window::window);
}

void Window::mouse_pos_cbk(GLFWwindow *_window, double x, double y) {
    // 更新鼠标位置到全局
    mouse_x = x;
    mouse_y = y;

    // 鼠标的值已经可以使用了
    mouse_init = true;
}

bool Window::need_close() {
    return glfwWindowShouldClose(Window::window);
}

void Window::update() {

    keyboard_events_check();
    close_window_check();
}

void Window::frame_buffer_cbk(GLFWwindow *_window, int _width, int _height) {
    width = _width;
    height = _height;
    glViewport(0, 0, width, height);
}

Window::Window() noexcept {
    assert(width > 0 && height > 0);
    SPDLOG_INFO("create window ({:d}, {:d})", width, height);

    // 创建窗口
    Window::window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        SPDLOG_ERROR("fail to create window.");
        glfwTerminate();
        exit(-1);
    }
}

void Window::close_window_check() {
    if (std::find(key_events.begin(), key_events.end(), KeyboardEvent::press_Esc) != key_events.end()) {
        SPDLOG_INFO("check ESC is pressed, window will be closed");
        glfwSetWindowShouldClose(window, true);
    }
}

void Window::init() {
    g_window = new Window();

    // 注册回调函数
    glfwSetCursorPosCallback(window, mouse_pos_cbk);
    glfwSetFramebufferSizeCallback(window, frame_buffer_cbk);
    glfwSetMouseButtonCallback(window, mouse_button_cbk);

    // 捕捉光标
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::mouse_button_cbk(GLFWwindow *_window, int button, int action, int mods) {
    static std::map<int, MouseButtonEvent> press_map{
            {GLFW_MOUSE_BUTTON_LEFT,  MouseButtonEvent::press_Left},
            {GLFW_MOUSE_BUTTON_RIGHT, MouseButtonEvent::press_Right},
    };

    for (auto const &iter : press_map) {
        if (button == iter.first) {
            auto event_iter = std::find(mouse_button_events.begin(), mouse_button_events.end(), iter.second);
            if (event_iter == mouse_button_events.end() && action == GLFW_PRESS)
                mouse_button_events.push_back(iter.second);
            if (event_iter != mouse_button_events.end() && action == GLFW_RELEASE)
                mouse_button_events.erase(event_iter);
            break;
        }
    }
}

void Window::keyboard_events_check() {
    // GLFW 的按键和自定义键盘事件的对应表
    static std::map<int, KeyboardEvent> keyboard_event_map{
            {GLFW_KEY_W,        KeyboardEvent::press_W},
            {GLFW_KEY_A,        KeyboardEvent::press_A},
            {GLFW_KEY_S,        KeyboardEvent::press_S},
            {GLFW_KEY_D,        KeyboardEvent::press_D},
            {GLFW_KEY_Q,        KeyboardEvent::press_Q},
            {GLFW_KEY_E,        KeyboardEvent::press_E},
            {GLFW_KEY_ESCAPE,   KeyboardEvent::press_Esc},
            {GLFW_KEY_LEFT_ALT, KeyboardEvent::press_LAlt},
    };

    // 查表，将 GLFW 的键盘封装为自定义的事件类型
    key_events.clear();
    for (const auto &iter : keyboard_event_map) {
        if (glfwGetKey(window, iter.first) == GLFW_PRESS)
            key_events.push_back(iter.second);
    }
}
