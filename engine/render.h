
#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

// 这个宏定义可以让 glfw 不必出现在 glad 的后面
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <memory>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "camera.h"


// =====================================================
// 使用方法：
//  Render.init();
//  Render.render<Scene>();
//  Render.terminate();
// =====================================================


class Render {
public:

    /* 渲染器初始化 */
    static void init() {

        // =====================================================
        // 环境初始化
        // 注：初始化顺序不能改变
        // glfw -> window -> glad -> imgui
        // =====================================================
        _spdlog_init();
        _glfw_init();
        Window::init("AccRender", 720, 16, 9);
        _glad_init();
        _imgui_init();
    }

    /* 渲染某个场景 */
    template<class SCENE>
    static void render() {

        /* 创建一个摄像机 */
        camera = std::make_shared<Camera>(16.f / 9.f);

        /* 场景初始化 */
        SCENE scene;
        scene.init();

        /* 帧速率统计相关的变量 */
        auto last_time = std::chrono::system_clock::now();
        const int frames_per_update = 60;       // 每 60 帧更新一次帧速率
        int frame_idx = 0;      // 每 60 帧统计一次，当前是第几帧

        /* 开始渲染 */
        glEnable(GL_DEPTH_TEST);
        while (!Window::should_close()) {
            /* 清空 buffer */
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* 窗口检测事件 */
            Window::update();

            /* 检查键盘，移动摄像机 */
            for (auto key_event: Window::key_events())
                if (CAMERA_KEY_MAP.find(key_event) != CAMERA_KEY_MAP.end())
                    camera->translate(CAMERA_KEY_MAP[key_event], 1.f);

            /* 检查鼠标位置，旋转摄像机 */
            if (auto iter = Window::mouse_button_events().find(MouseButton::Right);
                    iter != Window::mouse_button_events().end() && iter->second == ButtonEvent::Press) {
                auto[delta_x, delta_y] = Window::delta_mouse_pos();
                camera->rotate(std::abs(delta_x) < 0.01 ? 0 : (float) delta_x,
                               std::abs(delta_y) < 0.01 ? 0 : float(-delta_y));
            }

            /* 场景更新内容，渲染 */
            scene.update();

            /* 交换双缓冲 */
            glfwSwapBuffers(Window::window());

            /* 检测是否发生了错误 */
            _check_gl_error();

            /* 计算 frame rate */
            if (frame_idx++ > frames_per_update) {
                frame_idx = 0;
                auto cur_time = std::chrono::system_clock::now();
                auto delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_time).count();
                _frame_rate = frames_per_update * 1000.f / (float) delta_ms;
                last_time = cur_time;
            }
        }
    }

    /* 渲染器终止，回收资源 */
    static void terminate() {
        /* 销毁窗口 */
        Window::destroy();

        /* 回收 imgui 的资源 */
        _imgui_terminate();

        /* glfw 关闭 */
        glfwTerminate();
    }


    inline static float frame_rate() { return _frame_rate; }

public:
    static inline std::shared_ptr<Camera> camera{nullptr};


private:

    /* 帧率 frame per second */
    static inline float _frame_rate{0.f};

    /* 初始化日志 */
    static void _spdlog_init();

    /* glfw 初始化，指定 OpenGL 的版本号 */
    static void _glfw_init();

    /* glad 初始化 */
    static void _glad_init();

    /* GUI初始化：imgui */
    static void _imgui_init();

    /* GUI 关闭，销毁对象，回收资源 */
    static void _imgui_terminate();

    /* 调用 OpenGL 的接口，进行错误检测 */
    static void _check_gl_error();

    /* camera 的按键配置 */
    static inline std::map<KeyboardEvent, TransDirection> CAMERA_KEY_MAP{
            {KeyboardEvent::press_W, TransDirection::front},
            {KeyboardEvent::press_A, TransDirection::left},
            {KeyboardEvent::press_S, TransDirection::back},
            {KeyboardEvent::press_D, TransDirection::right},
            {KeyboardEvent::press_Q, TransDirection::up},
            {KeyboardEvent::press_E, TransDirection::down},
    };

    /* OpenGL 提供的错误码 */
    static inline const std::map<GLenum, std::string> GL_ERROR_MAP{
            {GL_INVALID_ENUM,                  "INVALID_ENUM"},
            {GL_INVALID_VALUE,                 "INVALID_VALUE"},
            {GL_INVALID_OPERATION,             "INVALID_OPERATION"},
            {GL_STACK_OVERFLOW,                "STACK_OVERFLOW"},
            {GL_STACK_UNDERFLOW,               "STACK_UNDERFLOW"},
            {GL_OUT_OF_MEMORY,                 "OUT_OF_MEMORY"},
            {GL_INVALID_FRAMEBUFFER_OPERATION, "INVALID_FRAMEBUFFER_OPERATION"},
    };

};


#endif //RENDER_RENDER_H
