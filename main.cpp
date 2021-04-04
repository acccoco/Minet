
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
    init_log();
    init_glfw();
    GLFWwindow *window = init_window(WINDOW_WIDTH, WINDOW_HEIGHT);
    init_glad();

    // 开启深度缓冲
    glEnable(GL_DEPTH_TEST);

    // 摄像机初始化
    Camera::init();

    // 场景初始化
    MyScene scene;
    scene.init();

    /* 渲染循环 */
    SPDLOG_INFO("start loop");
    while (!glfwWindowShouldClose(window)) {
        // 处理键盘事件
        process_esc(window);
        Camera::camera_move_loop(window);

        // 清屏
        glClearColor(0.f, 0.f, 0.f, 0.f);

        // 清除帧缓冲和深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 场景绘制
        scene.update();

        // 交换双缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
