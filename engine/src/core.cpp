#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "../core.h"
#include "../camera.h"

// 全局对象 =======================================================================

static double g_xpos_last;
static double g_ypos_last;


// 函数实现 =======================================================================

void init_glad() {
    SPDLOG_INFO("init glad");
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        SPDLOG_ERROR("fail to init glad");
        exit(-1);
    }
}


void init_glfw() {
    SPDLOG_INFO("init glfw");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void init_log() {
    spdlog::set_level(spdlog::level::debug);

    /**
     * %L 日志级别，缩写
     * %t 线程
     *
     * 以下选项必须要用 SPDLOG_INFO 才能生效
     * %s 文件的 basename
     * %# 行号
     * %! 函数名
     *
     * 格式控制：
     * %-4!<flag> 表示左对齐，4位，超出截断
     */
    spdlog::set_pattern("[%H:%M:%S][%^%L%$][%15!s:%-3!#][%!] %v");
}


/**
 * 判断按键，是否按下 Esc，退出渲染循环
 * @param window
 */
void process_esc(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/**
 * 窗口大小改变的回调函数
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


/**
 * 鼠标位置移动的回调
 */
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    // 第一次时，初始化鼠标的位置值
    static bool first_time = true;
    if (first_time) {
        first_time = false;
        g_xpos_last = xpos;
        g_ypos_last = ypos;
        return;
    }

    Camera::camera_rotate_callback(xpos - g_xpos_last, g_ypos_last - ypos);

    g_xpos_last = xpos;
    g_ypos_last = ypos;
}


GLFWwindow* init_window(int width, int height) {
    assert(width > 0 && height > 0);

    SPDLOG_INFO("create window ({:d}, {:d})", width, height);
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        SPDLOG_ERROR("fail to create window");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    SPDLOG_INFO("set context to window");

    // 主色窗口大小改变回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    SPDLOG_INFO("set window size change callback: sync to frame buffer");

    // 捕捉光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 注册鼠标回调
    glfwSetCursorPosCallback(window, mouse_callback);

    return window;
}
