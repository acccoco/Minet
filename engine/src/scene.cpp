#include "../scene.h"


std::shared_ptr<Camera> Scene::camera = nullptr;

void Scene::camera_rotate_cbk(double xpos, double ypos) {
    static bool first_time = true;
    static double xpos_last, ypos_last;

    // 初次执行
    if (first_time) {
        first_time = false;
        xpos_last = xpos;
        ypos_last = ypos;
    }

    // 正常执行
    camera->rotate(xpos - xpos_last, ypos_last - ypos);
    xpos_last = xpos;
    ypos_last = ypos;
}

void Scene::camera_translate_cbk(const std::vector<KeyboardEvent> &events) {
    // 键盘与摄像机的映射
    static std::map<KeyboardEvent, TransDirection> key_dir_map {
            {KeyboardEvent::press_W, TransDirection::front},
            {KeyboardEvent::press_A, TransDirection::left},
            {KeyboardEvent::press_S, TransDirection::back},
            {KeyboardEvent::press_D, TransDirection::right},
            {KeyboardEvent::press_Q, TransDirection::up},
            {KeyboardEvent::press_E, TransDirection::down},
    };

    // 找到当前键盘事件中，摄像机需要的键盘事件
    for (auto event: events) {
        if (key_dir_map.find(event) == key_dir_map.end())
            continue;
        // 根据方向进行移动，每次移动 1 个距离
        camera->translate(key_dir_map[event], 1.f);
    }
}
