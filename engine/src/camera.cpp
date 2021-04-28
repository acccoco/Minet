#include <map>

#include "../camera.h"
#include "../window.h"
#include "../utils/common.h"


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

    // 摄像机的前方，平行于地面的
    glm::vec3 ahead = glm::normalize(glm::cross(UP, cur_right));

    // 摄像机的相对方向在世界坐标系下的向量表示
    std::map<TransDirection, glm::vec3> dir_map{
            {TransDirection::front, ahead},
            {TransDirection::back,  -ahead},
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

void Camera::update() {
    // 获取全局窗口对象的鼠标位置，更新摄像机姿态
    this->do_rotate();

    // 获取窗口的键盘事件，更新摄像机的
    this->do_translate();
}

void Camera::do_translate() {
    // 键盘与摄像机的映射
    static std::map<KeyboardEvent, TransDirection> key_dir_map{
            {KeyboardEvent::press_W, TransDirection::front},
            {KeyboardEvent::press_A, TransDirection::left},
            {KeyboardEvent::press_S, TransDirection::back},
            {KeyboardEvent::press_D, TransDirection::right},
            {KeyboardEvent::press_Q, TransDirection::up},
            {KeyboardEvent::press_E, TransDirection::down},
    };

    // 找到当前键盘事件中，摄像机需要的键盘事件
    for (auto event: Window::key_events) {
        if (key_dir_map.find(event) == key_dir_map.end())
            continue;
        // 根据方向进行移动，每次移动 1 个距离
        this->translate(key_dir_map[event], 1.f);
    }
}

void Camera::do_rotate() {
    // 按下鼠标右键，旋转摄像机
    if (!vector_find(Window::mouse_button_events, MouseButtonEvent::press_Right)) {
        fist_rotate = true;
        return;
    }
    // 按下 Alt 才可以旋转摄像机
//    if (!vector_find(Window::key_events, KeyboardEvent::press_LAlt)) {
//        fist_rotate = true;
//        return;
//    }


    /**
     * 轮询方式的弊端：
     *  只有当第一次鼠标移动时，Window 的鼠标值才会更新，
     *  在这之前已经开始记录 xpos_last 和 ypos_last 了，且记录的值都是 0，是无效的
     *  当鼠标第一次移动是，真实的鼠标位置不是 0 这时会出现 xpos 和 ypos 的突变
     * 解决办法是：
     *  只有鼠标真正移动后，才开始记录 xpos_last 和 ypos_last
     */
    if (Window::mouse_init && fist_rotate) {
        fist_rotate = false;
        xpos_last = Window::mouse_x;
        ypos_last = Window::mouse_y;
        return;
    }

    // 如果坐标差距过小（小于 0.01 像素），就当没有移动
    float delta_x = Window::mouse_x - xpos_last;
    float delta_y = ypos_last - Window::mouse_y;
    if (abs(delta_x) < 0.01f && abs(delta_y) < 0.01f)
        return;

    this->rotate(delta_x, delta_y);
    xpos_last = Window::mouse_x;
    ypos_last = Window::mouse_y;
}
