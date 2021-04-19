/**
 * glad 和 glfw 的初始化
 */

#ifndef RENDER_ENGINE_INIT_H
#define RENDER_ENGINE_INIT_H

#include <GLFW/glfw3.h>

/* 初始化glad，必须在设置 glfw 的 context 之后调用 */
void init_glad();

/* 初始化glfw，设置版本等 */
void glfw_init();

/* 初始化日志的格式等 */
void logger_init();

void imgui_init();

void imgui_terminate();

#endif //RENDER_INIT_H
