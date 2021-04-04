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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>


// 常量定义
// ============================================================================

const float CAMERA_MOVE_SPEED = 0.05f;
const float CAMERA_ROTATE_SPEED = 0.05f;
const glm::vec3 UP = glm::vec3(0.f, 1.f, 0.f);


// 类型定义
// ============================================================================
// todo 重构摄像机
class Camera {
public:
    Camera() = default;

    /**
     * 根据摄像机的位姿，获得 view matrix
     */
    static glm::mat4 view_matrix();

    static glm::vec3 pos_get();

    static glm::vec3 get_front();

    /**
     * 根据摄像机的参数，获得 projection matrix
     */
    static glm::mat4 proj_matrix_get();

    /**
     * 窗口的回调，检查鼠标位置
     */
    static void camera_rotate_callback(double delta_x, double delta_y);

    static void init();

    // 每次渲染循环中调用，检查键盘是否按下
    static void camera_move_loop(GLFWwindow *window);

protected:
    static glm::vec3 pos;       // (world) 摄像机的位置
    static glm::vec3 front;     // (world) 摄像机的朝向

    static float yaw;           // 姿态：偏航角
    static float pitch;         // 姿态：俯仰角

    static glm::mat4 projection;    // 投影矩阵，只读的
};


#endif //RENDER_CAMERA_H
