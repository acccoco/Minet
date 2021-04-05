#include "../camera.h"

#include <map>


glm::mat4 Camera::view_matrix_get() {
    front.x = -cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    return glm::lookAt(this->position, this->position + glm::normalize(front), UP);
}

glm::vec3 Camera::position_get() {
    return this->position;
}

glm::vec3 Camera::front_dir_get() {
    return this->front;
}

glm::mat4 Camera::projection_matrix_get() {
    return this->projection;
}

void Camera::translate(TransDirection direction, float distance) {
    // 当前摄像机的右边是哪个方向
    glm::vec3 cur_right = glm::normalize(glm::cross(this->front, UP));

    // 摄像机的相对方向在世界坐标系下的向量表示
    std::map<TransDirection, glm::vec3> dir_map {
            {TransDirection::front, this->front},
            {TransDirection::back,  -this->front},
            {TransDirection::up,    UP},
            {TransDirection::down,  -UP},
            {TransDirection::left,  -cur_right},
            {TransDirection::right, cur_right},
    };

    // 进行移动
    auto iter = dir_map.find(direction);
    if (iter == dir_map.end())
        return;
    this->position += distance * CAMERA_MOVE_SPEED * iter->second;
}

void Camera::rotate(float delta_yaw, float delta_pitch) {
    this->yaw -= delta_yaw * CAMERA_ROTATE_SPEED;
    this->pitch += delta_pitch * CAMERA_ROTATE_SPEED;

    // 限制摄像机的俯仰角度
    if (this->pitch < -89.f)
        this->pitch = -89.f;
    else if (this->pitch > 89.f)
        this->pitch = 89.f;
}

Camera::Camera() {
    SPDLOG_INFO("_init camera.");

    this->projection = glm::perspective(glm::radians(this->fov), this->aspect, this->z_near, this->z_far);
}
