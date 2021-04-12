
#include <vector>

#include <glad/glad.h>
#include <assimp/Importer.hpp>

#include "./engine/core.h"
#include "./engine/framebuffer.h"
#include "./engine/scene.h"

#include "./scene/myscene.h"
#include "./scene/scene_nano.h"

std::shared_ptr<Shader> diffuse_shader;
std::shared_ptr<Shader> square_shader;
std::shared_ptr<Texture2D> lava_diffuse;
std::shared_ptr<Texture2D> box_diffuse;
std::shared_ptr<Camera> camera;

std::shared_ptr<FrameBuffer> frame_buffer;

unsigned int cube;
unsigned int plane;
unsigned int square;

unsigned int init_cube();

unsigned int init_plane();

unsigned int init_square();

/* 绘制场景，准备绘制到帧缓冲对象上 */
void draw_scene();

/* 绘制正方向 */
void draw_square(unsigned int texture_id);

int main() {
    /* 初始化 */
    logger_init();
    glfw_init();
    Window::init();
    Window::use();
    init_glad();

    // 摄像机
    camera = std::make_shared<Camera>();

    // 模型
    cube = init_cube();
    plane = init_plane();
    square = init_square();

    // shader
    diffuse_shader = std::make_shared<Shader>(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
    square_shader = std::make_shared<Shader>(SHADER("framebuffer.vert"), SHADER("framebuffer.frag"));

    // 纹理
    lava_diffuse = std::make_shared<Texture2D>(TEXTURE("lava/diffuse.tga"));
    box_diffuse = std::make_shared<Texture2D>(TEXTURE("container2.jpg"));

    // 帧缓冲
    frame_buffer = std::make_shared<FrameBuffer>(1600, 1200);

    SPDLOG_INFO("start loop");
    while (!Window::need_close()) {
        Window::update();
        camera->update();

        // 使用帧缓冲
        frame_buffer->use();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene();

        // 使用窗口系统的帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_square(frame_buffer->color_tex_get());

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }
    glfwTerminate();
}


void draw_square(unsigned int texture_id) {
    square_shader->use();

    glBindTexture(GL_TEXTURE_2D, texture_id);
    square_shader->uniform_tex2d_set("texture1", 0);

    glBindVertexArray(square);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
}

void draw_scene() {
    // 着色器的基本属性
    diffuse_shader->use();
    diffuse_shader->uniform_mat4_set("view", camera->view_matrix_get());
    diffuse_shader->uniform_mat4_set("projection", camera->projection_matrix_get());

    // 地面
    glBindVertexArray(plane);
    glBindTexture(GL_TEXTURE_2D, lava_diffuse->id);
    diffuse_shader->uniform_tex2d_set("texture1", 0);

    diffuse_shader->uniform_mat4_set("model", glm::one<glm::mat4>());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 立方体
    glBindVertexArray(cube);
    glBindTexture(GL_TEXTURE_2D, box_diffuse->id);
    diffuse_shader->uniform_tex2d_set("texture1", 0);

    glm::mat4 model_cube_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 0.01f, -1.f));
    diffuse_shader->uniform_mat4_set("model", model_cube_1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 model_cube_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 0.0f));
    diffuse_shader->uniform_mat4_set("model", model_cube_2);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}


float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};


float planeVertices[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
};


float square_vertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
};


unsigned int init_square() {
    // cube VAO
    unsigned int square_VAO, square_VBO;
    glGenVertexArrays(1, &square_VAO);
    glGenBuffers(1, &square_VBO);
    glBindVertexArray(square_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, square_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), &square_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    glBindVertexArray(0);
    return square_VAO;
}


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
