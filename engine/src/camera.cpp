#include "../camera.h"

glm::vec3 Camera::eye;
glm::vec3 Camera::front;
glm::mat4 Camera::projection;
float Camera::yaw;
float Camera::pitch;

glm::mat4 Camera::view_matrix() {
    front.x = -cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    return glm::lookAt(eye, eye + glm::normalize(front), UP);
}

glm::vec3 Camera::get_eye() {
    return Camera::eye;
}

glm::vec3 Camera::get_front() {
    return Camera::front;
}

glm::mat4 Camera::projection_matrix() {
    return projection;
}

void Camera::camera_rotate_callback(double delta_x, double delta_y) {
    Camera::yaw -= delta_x * CAMERA_ROTATE_SPEED;
    Camera::pitch += delta_y * CAMERA_ROTATE_SPEED;

    if (Camera::pitch < -89.f)
        Camera::pitch = -89.f;
    if (Camera::pitch > 89.f)
        Camera::pitch = 89.f;
}

void Camera::init() {
    LOG(INFO) << "global camera init.";

    yaw = 0;
    pitch = 0;
    eye = glm::vec3(0.f, 0.f, 0.f);
    front = glm::vec3(0.f, 0.f, -1.f);

    projection = glm::perspective(glm::radians(45.0f), 4 / (float) 3, 0.1f, 100.0f);
}

void Camera::camera_move_loop(GLFWwindow *window) {
    // 前后左右
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera::eye += CAMERA_MOVE_SPEED * Camera::front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera::eye -= CAMERA_MOVE_SPEED * Camera::front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera::eye -= glm::normalize(glm::cross(Camera::front, UP)) * CAMERA_MOVE_SPEED;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera::eye += glm::normalize(glm::cross(Camera::front, UP)) * CAMERA_MOVE_SPEED;

    // 上下
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        Camera::eye += CAMERA_MOVE_SPEED * UP;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        Camera::eye -= CAMERA_MOVE_SPEED * UP;
}
