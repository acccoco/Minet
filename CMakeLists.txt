cmake_minimum_required(VERSION 3.16)
project(render)


############################################################
# 编译选项
############################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


############################################################
# 系统一些头文件的位置
############################################################
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)


############################################################
# 第三方库
############################################################
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# imgui 依赖的库
find_library(COCOA_LIB Cocoa)
find_library(IOKit_LIB IOKit)
find_library(CoreVideo_LIB CoreVideo)

# glad
add_library(GLAD STATIC lib/glad/src/glad.c)
target_include_directories(GLAD PUBLIC lib/glad/include)

# imgui
add_library(IMGUI STATIC
        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp
        lib/imgui/imgui_demo.cpp)
target_include_directories(IMGUI PUBLIC lib/imgui)
target_link_libraries(IMGUI GLAD)

# stb_image 库
add_library(STBIMAGE STATIC lib/stb_image/src/stb_image.cpp)
target_include_directories(STBIMAGE PUBLIC lib/stb_image/include)


############################################################
# 所有的库文件
############################################################
set(LIB_LINKS
        spdlog::spdlog
        glm::glm
        glfw
        ${ASSIMP_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${COCOA_LIB}
        ${IOKit_LIB}
        ${CoreVideo_LIB}
        Threads::Threads
        GLAD
        IMGUI
        STBIMAGE
        )


############################################################
# 当前项目的源文件
############################################################
list(APPEND PRJ_SRCS
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


############################################################
# 渲染每个场景
############################################################
set(SCENES
        scene_box_floor
        scene_edge_outline
        scene_face_cull
        scene_post_process
        scene_light
        scene_nano
        scene_normal_visualize
        scene_pbr_dl
        scene_pbr_ibl
        scene_simple
        scene_skybox
        scene_space
        scene_transparent
        )
foreach(scene ${SCENES})
    add_executable(${scene} ${PRJ_SRCS} "scene/${scene}.cpp")
    target_link_libraries(${scene} ${LIB_LINKS})
endforeach()


# 一般测试
# =============================================================================
add_executable(test test.cpp)
target_link_libraries(test spdlog::spdlog)


# 单元用例
# =============================================================================
add_executable(test_delegate engine/test/test_delegate.cpp)
