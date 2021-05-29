/**
 * glad 和 glfw 的初始化
 */

#ifndef RENDER_ENGINE_INIT_H
#define RENDER_ENGINE_INIT_H

// 这个宏定义可以让 glfw 不必出现在 glad 的后面
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include "./window.h"

/* 初始化glad，必须在设置 glfw 的 context 之后调用 */
void init_glad();

/* 初始化glfw，设置版本等 */
void glfw_init();

/* 初始化日志的格式等 */
void logger_init();

void imgui_init();

void imgui_terminate();

template<class SCENE>
void run() {
    /* 配套环境初始化 */
    logger_init();
    glfw_init();
    Window::init();
    glfwMakeContextCurrent(Window::window);
    init_glad();
    imgui_init();

    // 场景初始化
    SCENE scene;
    scene.init();

    glEnable(GL_DEPTH_TEST);
    SPDLOG_INFO("start loop");
    while (!Window::need_close()) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Window::update();
        scene.update();

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }

    imgui_terminate();

    glfwDestroyWindow(Window::window);
    glfwTerminate();
}

#endif //RENDER_INIT_H
