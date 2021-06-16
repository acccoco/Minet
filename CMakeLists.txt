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
find_package(fmt REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# imgui 依赖的库
find_library(COCOA_LIB Cocoa)
find_library(IOKit_LIB IOKit)
find_library(CoreVideo_LIB CoreVideo)

# glad
add_library(LIB_GLAD STATIC lib/glad/src/glad.c)
target_include_directories(LIB_GLAD PUBLIC lib/glad/include)

# imgui
add_library(LIB_IMGUI STATIC
        lib/imgui/imgui.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(LIB_IMGUI PUBLIC lib/imgui)
target_link_libraries(LIB_IMGUI LIB_GLAD)

# stb_image 库
add_library(LIB_STBIMAGE STATIC lib/stb_image/src/stb_image.cpp)
target_include_directories(LIB_STBIMAGE PUBLIC lib/stb_image/include)

# 所有的库文件
set(LIB_LINKS
        glfw
        LIB_GLAD
        LIB_IMGUI
        glm::glm
        fmt::fmt
        LIB_STBIMAGE
        ${COCOA_LIB}
        ${IOKit_LIB}
        spdlog::spdlog
        ${CoreVideo_LIB}
        Threads::Threads
        ${ASSIMP_LIBRARIES}
        ${OPENGL_LIBRARIES})


############################################################
# render
############################################################
list(APPEND PRJ_SRCS
        engine/src/camera.cpp
        engine/src/frame_buffer.cpp
        engine/src/mesh.cpp
        engine/src/model.cpp
        engine/src/scene.cpp
        engine/src/shader.cpp
        engine/src/texture.cpp
        engine/src/window.cpp
        engine/src/render.cpp)

# 将engine 编译为静态库
add_library(engine STATIC ${PRJ_SRCS})
target_link_libraries(engine PUBLIC ${LIB_LINKS})
target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR}/engine)


############################################################
# 渲染每个测试场景
############################################################

# 注入头文件
configure_file(${CMAKE_SOURCE_DIR}/config.in ${CMAKE_BINARY_DIR}/config.hpp)

# 场景的定义
set(scenes
        box-floor
        border
        face-cull
        light
        nano-suit
        normal-visualize
        post-process
        simple
        sky-box
        transparent
        instanced-space
        pbr-direct-light
        pbr-image-based-light)

foreach (scene ${scenes})
    add_executable(example-${scene} examples/${scene}/main.cpp)
    target_link_libraries(example-${scene} engine)
    target_include_directories(example-${scene} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
endforeach()


# 一般测试
# =============================================================================
add_executable(test-1 test.cpp)
target_link_libraries(test-1 spdlog::spdlog)
