/**
 * glad 和 glfw 的初始化
 */

#ifndef RENDER_ENGINE_INIT_H
#define RENDER_ENGINE_INIT_H

#include <GLFW/glfw3.h>

/* 初始化glad，必须在设置 glfw 的 context 之后调用 */
void init_glad();

/* 初始化glfw */
void init_glfw();

void init_log();

GLFWwindow* init_window(int width, int height);

void process_esc(GLFWwindow *window);


#endif //RENDER_INIT_H
