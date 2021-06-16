

#ifndef RENDER_ENGINE_CAMERA_H
#define RENDER_ENGINE_CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>


/* 可以移动的方向 */
enum class TransDirection {
    front, back, up,
    down, left, right,
};


class Camera {
public:
    explicit Camera(float aspect = 16.f / 9.f, const glm::vec3 &position = {0.f, 0.f, 0.f})
            : _aspect(aspect), _position(position),
              _projection(glm::perspective(glm::radians(_fov), _aspect, _z_near, _z_far)) {}

    glm::mat4 view_matrix_get();

    [[nodiscard]] inline glm::vec3 position() const { return this->_position; }

    [[nodiscard]] inline glm::vec3 front() const { return this->_direction.front; }

    [[nodiscard]] inline glm::mat4 projection_matrix() const { return this->_projection; }

    /* 摄像机移动 */
    void translate(TransDirection direction, float distance);

    /* 摄像机旋转，可以传入鼠标的坐标差值，yaw 是 x 方向的；pitch 是 y 方向的 */
    void rotate(float delta_yaw, float delta_pitch);


private:
    const float _fov = 45.0f;       // field of view 视角
    const float _z_near = 0.1f;     // 近平面
    const float _z_far = 100.f;     // 远平面
    const float _aspect;            // 长宽比

    glm::vec3 _position;            // 摄像机的位置

    struct {
        glm::vec3 front{0.f, 0.f, -1.f};        // 摄像机的朝向
        float yaw = 0.f;                                        // 欧拉角：偏航
        float pitch = 0.f;                                      // 欧拉角：俯仰
    } _direction;

    const glm::mat4 _projection;                                 // 摄像机的投影矩阵

    const glm::vec3 GLOBAL_Y{0.f, 1.f, 0.f};      // 摄像机的上方向
    const float CAMERA_MOVE_SPEED = 0.05f;                  // 摄像机移动速度
    const float CAMERA_ROTATE_SPEED = 0.05f;                // 摄像机旋转速度

};


#endif //RENDER_CAMERA_H
