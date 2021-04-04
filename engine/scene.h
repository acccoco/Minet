#ifndef RENDER_ENGINE_SCENE_H
#define RENDER_ENGINE_SCENE_H

#include <memory>

#include <glm/glm.hpp>


class Scene {
public:
    /* 初始化场景, 可以用于初始化光源,摄像机等 */
    virtual void init() = 0;
    /* 每一帧会做哪些事情 */
    virtual void update() = 0;
};


#endif //RENDER_SCENE_H
