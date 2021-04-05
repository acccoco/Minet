/**
 * 定义摄像机，摄像机是一个全局对象
 *       ^ up
 *       |
 *       |
 *     __|___
 *    |\    /|
 *    | \__/ |
 *    |/  /|\|
 *    /__/ /   ------------> right
 *    |__|/
 *     /
 *    /
 *   /
 *  L  eye - target = -front
 *
 *
 * 设欧拉角为 (0, 0, 0) 时，摄像机的 front 为(0, 0, -1)
 */


#ifndef RENDER_ENGINE_CAMERA_H
#define RENDER_ENGINE_CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>


/* 可以移动的方向 */
enum class TransDirection {
    front,
    back,
    up,
    down,
    left,
    right,
};

class Camera {
public:
    Camera();

    glm::mat4 view_matrix_get();
    glm::vec3 position_get();
    glm::vec3 front_dir_get();
    glm::mat4 projection_matrix_get();

    void translate(TransDirection direction, float distance);

    /* 旋转，可以传入鼠标的坐标差值，yaw 是 x 方向的；pitch 是 y 方向的 */
    void rotate(float delta_yaw, float delta_pitch);


private:
    // field of view 视角
    const float fov = 45.0f;
    const float z_near = 0.1f;
    const float z_far = 100.f;
    // 长宽比
    const float aspect = 4.f / 3.f;
    const glm::vec3 UP{0.f, 1.f, 0.f};
    const float CAMERA_MOVE_SPEED = 0.05f;
    const float CAMERA_ROTATE_SPEED = 0.05f;

    glm::vec3 position{0.f, 0.f, 0.f};
    glm::vec3 front{0.f, 0.f, -1.f};

    float yaw = 0.f;
    float pitch = 0.f;

    glm::mat4 projection{};
};


#endif //RENDER_CAMERA_H
