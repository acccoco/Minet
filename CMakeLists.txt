cmake_minimum_required(VERSION 3.17)
project(render)


set(CMAKE_CXX_STANDARD 11)

set(ALL_SRC)
set(LINKS)

set(LIB_DIR "C:/Users/bigso/lib")

# ============================== lib ==========================================
# glad
# 用于寻找 OpenGL 的各个函数指针
set(GLAD "${LIB_DIR}/glad")
include_directories(${GLAD}/include)
file(GLOB GLAD_SRC "${GLAD}/src/*.c")
list(APPEND ALL_SRC ${GLAD_SRC})


# glfw
# 提供了显示窗口，事件支持
set(GLFW "${LIB_DIR}/GLFW")
include_directories(${GLFW}/include)
link_directories(${GLFW}/lib)
list(APPEND LINKS glfw3)


# opengl
find_package(OpenGL REQUIRED)
list(APPEND LINKS ${OPENGL_LIBRARIES})


# std_image
# 图片编辑的库
set(STD_IMAGE "${LIB_DIR}/stb_image")
include_directories(${STD_IMAGE}/include)
file(GLOB STD_IMAGE_SRC "${STD_IMAGE}/src/*.cpp")
list(APPEND ALL_SRC ${STD_IMAGE_SRC})


# easyloggingpp
# 日志库
set(EASY_LOG "${LIB_DIR}/easyloggingpp")
include_directories(${EASY_LOG}/include)
aux_source_directory("${EASY_LOG}/src" EASY_LOG_SRC)
list(APPEND ALL_SRC ${EASY_LOG_SRC})


# glm
# OpenGL 使用的矩阵库
set(GLM "${LIB_DIR}/glm")
include_directories(${GLM})


# assimp
set(ASSIMP "C:/Program Files/Assimp")
include_directories(${ASSIMP}/include)
link_directories(${ASSIMP}/lib/x64)
list(APPEND LINKS assimp)

# =============================================================================


# 当前项目
file(GLOB ENGINE_SRC "engine/src/*.cpp")
file(GLOB ENGINE_UTILS_SRC "engine/utils/src/*.cpp")
list(APPEND ALL_SRC ${ENGINE_SRC} ${ENGINE_UTILS_SRC})


# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")


# 目标1
add_executable(main ${ALL_SRC} main.cpp)  # 和glad的源文件一起编译
target_link_libraries(main ${LINKS})   # 链接opengl和glfw


# 目标2
add_executable(test test.cpp)
