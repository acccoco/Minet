
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "./engine/core.h"
#include "./engine/scene.h"
#include "./scene/scene_light.h"
#include "./scene/scene_simple.h"
#include "./scene/scene_nano.h"
#include "./scene/scene_space.h"
#include "./scene/scene_box_floor.h"
#include "./scene/scene_pbr_direct_light.h"


int main() {
    /* 配套环境初始化 */
    logger_init();
    glfw_init();
    Window::init();
    glfwMakeContextCurrent(Window::window);
    init_glad();
    imgui_init();

    // 场景初始化
    ScenePbrDirectLight scene;
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
