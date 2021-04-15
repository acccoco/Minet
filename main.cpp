
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


int main() {
    /* 初始化 */
    logger_init();
    glfw_init();
    Window::init();
    Window::use();
    init_glad();

    // 场景初始化
    SceneSpace scene;
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
    glfwTerminate();
}
