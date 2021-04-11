
#include <vector>

#include <glad/glad.h>
#include <assimp/Importer.hpp>

#include "./engine/core.h"
#include "./engine/scene.h"

#include "./scene/myscene.h"
#include "./scene/scene_nano.h"


unsigned int init_cube();

unsigned int init_plane();

unsigned int init_grass();

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
    GLuint grass = init_grass();

    // shader
    auto shader = Shader(SHADER("diffuse.vert"), SHADER("blend.frag"));

    // 纹理
    auto lava_diffuse = Texture2D(TEXTURE("lava/diffuse.tga"));
    auto box_diffuse = Texture2D(TEXTURE("container2.jpg"));
    auto grass_diffuse = Texture2D(TEXTURE("grass.png"), false);

    glEnable(GL_DEPTH_TEST);
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
        shader.uniform_tex2d_set("texture1", 0);

        // 地面
        glBindVertexArray(plane);
        glBindTexture(GL_TEXTURE_2D, lava_diffuse.id);

        shader.uniform_mat4_set("model", glm::one<glm::mat4>());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 立方体
        glBindVertexArray(cube);
        glBindTexture(GL_TEXTURE_2D, box_diffuse.id);

        glm::mat4 model_cube_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 0.01f, -1.f));
        shader.uniform_mat4_set("model", model_cube_1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 model_cube_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 0.0f));
        shader.uniform_mat4_set("model", model_cube_2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 植被
        glBindVertexArray(grass);
        glBindTexture(GL_TEXTURE_2D, grass_diffuse.id);

        std::vector<glm::vec3> vegetation{
                glm::vec3(-1.5f, 0.0f, -0.48f),
                glm::vec3(1.5f, 0.0f, 0.51f),
                glm::vec3(0.0f, 0.0f, 0.7f),
                glm::vec3(-0.3f, 0.0f, -2.3f),
                glm::vec3(0.5f, 0.0f, -0.6f)
        };
        glm::mat4 model_grass;
        for (auto & i : vegetation) {
            model_grass = glm::translate(glm::one<glm::mat4>(), i);
            shader.uniform_mat4_set("model", model_grass);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }
    glfwTerminate();
}


// 正方体的模型
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


// 平面的模型
float planeVertices[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
};


// 四边形面片：植物草
float grassVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f, 0.0f
};

unsigned int init_grass() {
    unsigned int grass_VAO, grass_VBO;
    glGenVertexArrays(1, &grass_VAO);
    glGenBuffers(1, &grass_VBO);
    glBindVertexArray(grass_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, grass_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));

    glBindVertexArray(0);
    return grass_VAO;
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
