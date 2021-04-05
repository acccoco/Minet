#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "../core.h"


void init_glad() {
    SPDLOG_INFO("_init glad");
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        SPDLOG_ERROR("fail to _init glad");
        exit(-1);
    }
}


void glfw_init() {
    SPDLOG_INFO("_init glfw");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void logger_init() {
    spdlog::set_level(spdlog::level::debug);

    /**
     * %L 日志级别，缩写
     * %t 线程
     *
     * 以下选项必须要用 SPDLOG_INFO 才能生效
     * %s 文件的 basename
     * %# 行号
     * %! 函数名
     *
     * 格式控制：
     * %-4!<flag> 表示左对齐，4位，超出截断
     */
    spdlog::set_pattern("[%H:%M:%S][%^%L%$][%15!s:%-3!#][%!] %v");
}
