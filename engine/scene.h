#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include <memory>
#include <exception>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "./camera.h"
#include "./window.h"


class Scene {
public:
    /* 初始化场景, 可以用于初始化光源,摄像机等 */
    virtual void _init() = 0;

    /* 每一帧会做哪些事情 */
    virtual void _update() = 0;

    void camera_bind(const std::shared_ptr<Camera> &_camera) {
        this->camera = _camera;
    }

    void init(Window &window) {
        this->_init();

        // 检查是否绑定了摄像机
        if (!this->camera) {
            SPDLOG_ERROR("camera must be bind to scene.");
            throw std::exception();
        }

        // 绑定鼠标回调函数
        window.mouse_pos_delegates.add(new FuncDelegate<double, double>(camera_rotate_cbk));

        // 绑定键盘回调函数
        window.keyboard_delegates.add(new FuncDelegate<const std::vector<KeyboardEvent> &>(camera_translate_cbk));
    }

    void update() {
        this->_update();
    }

private:
    static std::shared_ptr<Camera> camera;

    /* 摄像机移动的回调，（将键盘按键转换为摄像机的方向） */
    static void camera_translate_cbk(const std::vector<KeyboardEvent> &events);

    /* 摄像机旋转的回调，（将鼠标坐标转换为摄像机的方位角） */
    static void camera_rotate_cbk(double xpos, double ypos);

};

#endif //RENDER_SCENE_H
