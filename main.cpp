
#include <vector>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "./engine/core.h"
#include "./engine/scene.h"

#include "./scene/myscene.h"
#include "./scene/scene2.h"
#include "./scene/scene_nano.h"


const unsigned int WINDOW_WIDTH = 1600;
const unsigned int WINDOW_HEIGHT = 1200;


int main() {
    /* 初始化 */
    logger_init();
    glfw_init();
    Window window;
    window.use();
    init_glad();

    // 场景初始化
    MyScene scene;
    scene.init(window);

    glEnable(GL_DEPTH_TEST);
    SPDLOG_INFO("start loop");
    while (!window.need_close()) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.update();

        scene.update();

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
    glfwTerminate();
}
