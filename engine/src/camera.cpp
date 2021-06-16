#include <map>

#include "camera.h"
#include "window.h"


glm::mat4 Camera::view_matrix_get() {
    _direction.front.x = -cos(glm::radians(_direction.pitch)) * sin(glm::radians(_direction.yaw));
    _direction.front.y = sin(glm::radians(_direction.pitch));
    _direction.front.z = -cos(glm::radians(_direction.pitch)) * cos(glm::radians(_direction.yaw));
    return glm::lookAt(this->_position, this->_position + glm::normalize(_direction.front), GLOBAL_Y);
}


void Camera::translate(TransDirection direction, float distance) {
    // 当前摄像机的右边是哪个方向
    glm::vec3 cur_right = glm::normalize(glm::cross(this->_direction.front, GLOBAL_Y));

    // 摄像机的前方，平行于地面的
    glm::vec3 ahead = glm::normalize(glm::cross(GLOBAL_Y, cur_right));

    // 摄像机的相对方向在世界坐标系下的向量表示
    std::map<TransDirection, glm::vec3> dir_map{
            {TransDirection::front, ahead},
            {TransDirection::back,  -ahead},
            {TransDirection::up,    GLOBAL_Y},
            {TransDirection::down,  -GLOBAL_Y},
            {TransDirection::left,  -cur_right},
            {TransDirection::right, cur_right},
    };

    // 进行移动
    auto iter = dir_map.find(direction);
    if (iter == dir_map.end())
        return;
    this->_position += distance * CAMERA_MOVE_SPEED * iter->second;
}

void Camera::rotate(float delta_yaw, float delta_pitch) {
    this->_direction.yaw -= delta_yaw * CAMERA_ROTATE_SPEED;
    this->_direction.pitch += delta_pitch * CAMERA_ROTATE_SPEED;

    // 限制摄像机的俯仰角度
    if (this->_direction.pitch < -89.f)
        this->_direction.pitch = -89.f;
    else if (this->_direction.pitch > 89.f)
        this->_direction.pitch = 89.f;
}
