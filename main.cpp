
#include <vector>

#include <glad/glad.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>

#include "./engine/core.h"
#include "./engine/scene.h"

#include "./scene/myscene.h"
#include "./scene/scene_nano.h"


unsigned int init_cube();

unsigned int init_plane();

unsigned int init_skybox();

unsigned int cubemap_gen();

unsigned int init_box_normal();

unsigned int uniform_block_get();

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
    GLuint skybox = init_skybox();
    GLuint box_normal = init_box_normal();

    // shader
    auto diffuse_shader = Shader(SHADER("diffuse.vert"), SHADER("diffuse.frag"));
    auto skybox_shader = Shader(SHADER("sky.vert"), SHADER("sky.frag"));
    auto reflect_shader = Shader(SHADER("reflect.vert"), SHADER("reflect.frag"));
    GLuint uniform_block_diffuse = glGetUniformBlockIndex(diffuse_shader.id, "Matrices");
    GLuint uniform_block_skybox = glGetUniformBlockIndex(skybox_shader.id, "Matrices");
    GLuint uniform_block_reflect = glGetUniformBlockIndex(reflect_shader.id, "Matrices");
    glUniformBlockBinding(diffuse_shader.id, uniform_block_diffuse, 0);
    glUniformBlockBinding(skybox_shader.id, uniform_block_skybox, 0);
    glUniformBlockBinding(reflect_shader.id, uniform_block_reflect, 0);

    // 创建 uniform block buffer
    GLuint ubo_matrices = uniform_block_get();
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->projection_matrix_get()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 纹理
    auto lava_diffuse = Texture2D(TEXTURE("lava/diffuse.tga"));
    auto box_diffuse = Texture2D(TEXTURE("container2.jpg"));
    auto skybox_tex = cubemap_gen();

    glEnable(GL_DEPTH_TEST);
    SPDLOG_INFO("start loop");
    while (!Window::need_close()) {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Window::update();
        camera->update();

        // 着色器的基本属性
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->view_matrix_get()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // 地面
        diffuse_shader.use();
        glBindVertexArray(plane);
        glBindTexture(GL_TEXTURE_2D, lava_diffuse.id);
        diffuse_shader.uniform_tex2d_set("texture1", 0);
        diffuse_shader.uniform_mat4_set("model", glm::one<glm::mat4>());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 立方体
        diffuse_shader.use();
        glBindVertexArray(cube);
        glBindTexture(GL_TEXTURE_2D, box_diffuse.id);
        diffuse_shader.uniform_tex2d_set("texture1", 0);

        glm::mat4 model_cube_1 = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 0.01f, -1.f));
        diffuse_shader.uniform_mat4_set("model", model_cube_1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 model_cube_2 = glm::translate(glm::one<glm::mat4>(), glm::vec3(2.0f, 0.01f, 0.0f));
        diffuse_shader.uniform_mat4_set("model", model_cube_2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 反射效果的盒子
        reflect_shader.use();
        glBindVertexArray(box_normal);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);
        reflect_shader.uniform_float_set("sky_texture", 0);
        glm::mat4 model_box = glm::translate(glm::one<glm::mat4>(), glm::vec3(-1.f, 1.f, 1.f));
        reflect_shader.uniform_mat4_set("model", model_box);
        reflect_shader.uniform_vec3_set("eye_pos", camera->position_get());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // 天空盒
        // 最后渲染天空盒，让天空盒的深度始终为最大
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skybox_shader.use();
        glBindVertexArray(skybox);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);
        skybox_shader.uniform_float_set("texture1", 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(Window::window);
        glfwPollEvents();
    }
    glfwTerminate();
}


unsigned int uniform_block_get() {
    unsigned int ubo_matrices;
    glGenBuffers(1, &ubo_matrices);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return ubo_matrices;
}

unsigned int cubemap_gen() {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    std::vector<std::pair<GLuint, std::string>> tex_conf{
            {GL_TEXTURE_CUBE_MAP_POSITIVE_X, TEXTURE("skybox/sky_Right.png")},       // 右，右手系
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, TEXTURE("skybox/sky_Left.png")},       // 左
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, TEXTURE("skybox/sky_Up.png")},       // 上
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, TEXTURE("skybox/sky_Down.png")},       // 下
            {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, TEXTURE("skybox/sky_Back.png")},       // 后
            {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, TEXTURE("skybox/sky_Front.png")},       // 前
    };
    int width, height, nr_channels;
    unsigned char *data;
    for (const auto &iter : tex_conf) {
        data = stbi_load(iter.second.c_str(), &width, &height, &nr_channels, 0);
        if (!data) {
            SPDLOG_ERROR("fail to load skybox texture, {}", iter.second);
            throw std::exception();
        }
        GLuint format = nr_channels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(iter.first, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    // 多级纹理
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // uv 超过后如何采样
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
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


float skybox_vertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};

float box_normal_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};


float planeVertices[] = {
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f
};

unsigned int init_box_normal() {
    unsigned int box_VAO, box_VBO;
    glGenVertexArrays(1, &box_VAO);
    glGenBuffers(1, &box_VBO);
    glBindVertexArray(box_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, box_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_normal_vertices), &box_normal_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));

    glBindVertexArray(0);
    return box_VAO;
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


unsigned int init_skybox() {
    unsigned int sky_VAO, sky_VBO;
    glGenVertexArrays(1, &sky_VAO);
    glGenBuffers(1, &sky_VBO);
    glBindVertexArray(sky_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sky_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);

    glBindVertexArray(0);
    return sky_VAO;
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
