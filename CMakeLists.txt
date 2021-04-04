cmake_minimum_required(VERSION 3.17)
project(render)

# 依赖库
# =============================================================================
# OpenGL 自带的
# glm：msys
# spdlog：msys
# glfw：msys
# assimp：msys
# glad：源代码
# stb_image ：源代码
# =============================================================================


# 编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")


# 文件夹的定义
set(LIB_DIR "C:/Users/bigso/lib")
set(MINGW64 "C:/msys64/mingw64")


# 找到 pkg
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)


# 链接库
set(LINKS)
list(APPEND LINKS
        glfw3
        spdlog::spdlog
        assimp
        ${OPENGL_LIBRARIES})


# 头文件
include_directories(${MINGW64}/include)
include_directories(
        ${LIB_DIR}/glad/include
        ${LIB_DIR}/stb_image/include
)


# 一起编译的源文件
set(ALL_SRC)
list(APPEND ALL_SRC
        ${LIB_DIR}/glad/src/glad.c
        ${LIB_DIR}/stb_image/src/stb_image.cpp
        )


# 当前项目的源文件
list(APPEND ALL_SRC
        engine/src/camera.cpp
        engine/src/color.cpp
        engine/src/core.cpp
        engine/src/light.cpp
        engine/src/mesh.cpp
        engine/src/model.cpp
        engine/src/shader.cpp
        engine/src/texture.cpp
        engine/src/window.cpp
        engine/utils/src/file.cpp)


# 目标1
# =============================================================================
add_executable(main ${ALL_SRC} main.cpp)
target_link_libraries(main ${LINKS})


# 目标2
# =============================================================================
add_executable(test test.cpp)
target_link_libraries(test spdlog::spdlog)
