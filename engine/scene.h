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
private:
    /* 场景自定义的初始化 */
    virtual void _init() = 0;

    /* 场景自定义的更新 */
    virtual void _update() = 0;

public:
    void init() {
        this->_init();
    }

    void update() {
        this->_update();
    }
};

#endif //RENDER_SCENE_H
