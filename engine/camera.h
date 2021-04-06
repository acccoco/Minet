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

    /* 摄像机更新 */
    void update();

private:
    const float fov = 45.0f;                                // field of view 视角
    const float z_near = 0.1f;                              // 近平面
    const float z_far = 100.f;                              // 远平面
    const float aspect = 4.f / 3.f;                         // 长宽比
    const glm::vec3 UP{0.f, 1.f, 0.f};      // 摄像机的上方向
    const float CAMERA_MOVE_SPEED = 0.05f;                  // 摄像机移动速度
    const float CAMERA_ROTATE_SPEED = 0.05f;                // 摄像机旋转速度
    glm::vec3 position{0.f, 0.f, 0.f};      // 摄像机的位置
    glm::vec3 front{0.f, 0.f, -1.f};        // 摄像机的朝向
    float yaw = 0.f;                                        // 欧拉角：偏航
    float pitch = 0.f;                                      // 欧拉角：俯仰
    glm::mat4 projection{};                                 // 摄像机的投影矩阵

    double xpos_last{}, ypos_last{};                // 上次的鼠标位置
    bool fist_rotate = true;                    // 摄像机是否是第一次旋转

    // 获取窗口的键盘事件，更新摄像机的
    void do_translate();

    // 获取全局窗口对象的鼠标位置，更新摄像机姿态
    void do_rotate();

    /* 摄像机移动 */
    void translate(TransDirection direction, float distance);

    /* 旋转，可以传入鼠标的坐标差值，yaw 是 x 方向的；pitch 是 y 方向的 */
    void rotate(float delta_yaw, float delta_pitch);

};


#endif //RENDER_CAMERA_H
