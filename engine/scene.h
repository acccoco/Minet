#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include <memory>
#include <exception>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "./camera.h"
#include "./window.h"


class Scene {
private:
    /* 场景自定义的初始化 */
    virtual void _init() = 0;

    /* 场景自定义的更新 */
    virtual void _update() = 0;

    virtual void _gui() {}

public:
    void init() {
        this->_init();
    }

    void update() {
        this->_update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        this->_gui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif //RENDER_SCENE_H
