cmake_minimum_required(VERSION 3.17)
project(render)

# 依赖库
# =============================================================================
# 自带的：
# OpenGL
#
# 通过 MSYS2 安装：
# glm
# spdlog
# glfw
# assimp
#
# 源代码：
# glad
# stb_image
# imgui
# =============================================================================


# 编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
set(THREADS_PREFER_PTHREAD_FLAG ON)


# 找到 pkg
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)


# 链接库
set(LINKS)
list(APPEND LINKS
        glfw3
        spdlog::spdlog
        assimp
        ${OPENGL_LIBRARIES}
        Threads::Threads
        # imgui 需要的
        imm32 dwmapi)


# 头文件
include_directories(C:/msys64/mingw64/include)
include_directories(
        lib/glad/include
        lib/stb_image/include
        lib/imgui
)


# 一起编译的源文件
set(ALL_SRC)
list(APPEND ALL_SRC
        lib/glad/src/glad.c

        lib/stb_image/src/stb_image.cpp

        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp
        lib/imgui/imgui_demo.cpp
        )


# 当前项目的源文件
list(APPEND ALL_SRC
        engine/src/camera.cpp
        engine/src/color.cpp
        engine/src/core.cpp
        engine/src/frame_buffer.cpp
        engine/src/light.cpp
        engine/src/mesh.cpp
        engine/src/model.cpp
        engine/src/scene.cpp
        engine/src/shader.cpp
        engine/src/texture.cpp
        engine/src/window.cpp
        engine/utils/src/file.cpp)


# 渲染各个场景
# =============================================================================
add_executable(scene_box_floor ${ALL_SRC} scene/scene_box_floor.cpp)
target_link_libraries(scene_box_floor ${LINKS})

add_executable(scene_post_process ${ALL_SRC} scene/scene_post_process.cpp)
target_link_libraries(scene_post_process ${LINKS})

add_executable(scene_light ${ALL_SRC} scene/scene_light.cpp)
target_link_libraries(scene_light ${LINKS})

add_executable(scene_nano ${ALL_SRC} scene/scene_nano.cpp)
target_link_libraries(scene_nano ${LINKS})

add_executable(scene_pbr_dl ${ALL_SRC} scene/scene_pbr_dl.cpp)
target_link_libraries(scene_pbr_dl ${LINKS})

add_executable(scene_pbr_ibl ${ALL_SRC} scene/scene_pbr_ibl.cpp)
target_link_libraries(scene_pbr_ibl ${LINKS})

add_executable(scene_simple ${ALL_SRC} scene/scene_simple.cpp)
target_link_libraries(scene_simple ${LINKS})

add_executable(scene_skybox ${ALL_SRC} scene/scene_skybox.cpp)
target_link_libraries(scene_skybox ${LINKS})

add_executable(scene_space ${ALL_SRC} scene/scene_space.cpp)
target_link_libraries(scene_space ${LINKS})

# 一般性测试
# =============================================================================
add_executable(test test.cpp)
target_link_libraries(test spdlog::spdlog)


# 测试用例
# =============================================================================
add_executable(test_delegate engine/test/test_delegate.cpp)
