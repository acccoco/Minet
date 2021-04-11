
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

    // 纹理
    auto lava_diffuse = Texture2D(TEXTURE("lava/diffuse.tga"));
    auto box_diffuse = Texture2D(TEXTURE("container2.jpg"));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    SPDLOG_INFO("start loop");
    while (!Window::need_close()) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Window::update();
        camera->update();

        // 着色器的基本属性
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

        // 立方体
        glBindVertexArray(cube);
        glBindTexture(GL_TEXTURE_2D, box_diffuse.id);
        shader.uniform_tex2d_set("texture1", 0);

        glm::mat4 model_cube_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 0.01f, -1.f));
        shader.uniform_mat4_set("model", model_cube_1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 model_cube_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 0.0f));
        shader.uniform_mat4_set("model", model_cube_2);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }
    glfwTerminate();
}


float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f  // bottom-left
};


float planeVertices[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

    glBindVertexArray(0);
    return plane_VAO;
}
