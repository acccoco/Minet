
#include <vector>

#include <glad/glad.h>
#include <assimp/Importer.hpp>

#include "./engine/core.h"
#include "./engine/scene.h"

#include "./scene/myscene.h"
#include "./scene/scene_nano.h"



unsigned int init_cube();
unsigned int init_plane();

int main() {
    /* 初始化 */
    logger_init();
    glfw_init();
    Window::init();
    Window::use();
    init_glad();

    // 摄像机
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    // 模型
    GLuint cube = init_cube();
    GLuint plane = init_plane();

    // shader
    auto shader = Shader(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
    auto single_color_shader = Shader(SHADER("diffuse.vert"), SHADER("single_color.frag"));

    // 纹理
    auto lava_diffuse = Texture2D(TEXTURE("lava/diffuse.tga"));
    auto box_diffuse = Texture2D(TEXTURE("container2.jpg"));

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    SPDLOG_INFO("start loop");
    while (!Window::need_close()) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Window::update();
        camera->update();

        // 着色器的基本属性
        single_color_shader.use();
        single_color_shader.uniform_mat4_set("view", camera->view_matrix_get());
        single_color_shader.uniform_mat4_set("projection", camera->projection_matrix_get());
        shader.use();
        shader.uniform_mat4_set("view", camera->view_matrix_get());
        shader.uniform_mat4_set("projection", camera->projection_matrix_get());

        // 地面
        glBindVertexArray(plane);
        glBindTexture(GL_TEXTURE_2D, lava_diffuse.id);
        shader.uniform_tex2d_set("texture1", 0);
        shader.uniform_mat4_set("model", glm::one<glm::mat4>());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 绘制普通立方体
        glBindVertexArray(cube);
        glBindTexture(GL_TEXTURE_2D, box_diffuse.id);
        shader.uniform_tex2d_set("texture1", 0);
        glm::mat4 model_cube_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 0.01f, -1.f));
        shader.uniform_mat4_set("model", model_cube_1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制有边框的立方体
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

        glm::mat4 model_cube_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 0.0f));
        shader.uniform_mat4_set("model", model_cube_2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 绘制外边框
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        single_color_shader.use();
        float scale = 1.05f;
        glm::mat4 model_cube_2_scale = glm::scale(model_cube_2, glm::vec3(scale, scale, scale));
        single_color_shader.uniform_mat4_set("model", model_cube_2_scale);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 恢复环境
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0xFF);        // 关键步骤：让接下来的 glClear 可以运行

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }
    glfwTerminate();
}


float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


float planeVertices[] = {
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};


unsigned int init_cube() {
    // cube VAO
    unsigned int cube_VAO, cube_VBO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    glBindVertexArray(cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    return cube_VAO;
}


unsigned int init_plane() {
    // plane VAO
    unsigned int plane_VAO, plane_VBO;
    glGenVertexArrays(1, &plane_VAO);
    glGenBuffers(1, &plane_VBO);
    glBindVertexArray(plane_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    return plane_VAO;
}
