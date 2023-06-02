//
// Created by bigso on 2021/4/28.
//

#ifndef RENDER_SPHEAR_H
#define RENDER_SPHEAR_H

#include <cmath>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Sphere {
public:

    Sphere() {
        sphere_gen(64, 32);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_STRIP, index_cnt, GL_UNSIGNED_INT, nullptr);
    }

    /**
     * 生成 UV sphere
     * y 方向：从北极到南极
     * x 方向：左手螺旋
     */
    void sphere_gen(int x_slice, int y_slice) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        // 生成顶点，北极和南极有很多个顶点，可以视为一个圆柱的侧面
        for (int i = 0; i <= y_slice; ++i) {
            float theta = glm::pi<float>() * i / y_slice;
            for (int j = 0; j <= x_slice; ++j) {
                float phi = 2 * glm::pi<float>() * j / x_slice;

                float x = std::sin(theta) * std::cos(phi);
                float y = std::cos(theta);
                float z = std::sin(theta) * std::sin(phi);

                positions.emplace_back(x, y, z);
                uvs.emplace_back(1 - float(j) / x_slice, 1 - float(i) / y_slice);
                normals.emplace_back(x, y, z);
            }
        }

        // 生成顶点索引，顶点顺序类似于弹簧
        for (int i = 0; i < y_slice; ++i) {
            for (int j = 0; j <= x_slice; ++j) {
                indices.push_back(i * (x_slice + 1) + j);
                indices.push_back((i + 1) * (x_slice + 1) + j);
            }
        }

        vertex_cnt = positions.size();
        index_cnt = indices.size();

        GLuint VBO_pos, VBO_uv, VBO_normal, EBO;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &VBO_pos);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

        glGenBuffers(1, &VBO_normal);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

        glGenBuffers(1, &VBO_uv);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_uv);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    GLuint VAO{};
    unsigned int vertex_cnt{};
    unsigned int index_cnt{};
};


#endif //RENDER_SPHEAR_H
