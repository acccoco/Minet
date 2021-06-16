#ifndef RENDER_COLOR_H
#define RENDER_COLOR_H

#include <glm/glm.hpp>


/* 颜色相关的定义 */
class Color {
public:
    inline static const glm::vec3 white{1.f, 1.f, 1.f};
    inline static const glm::vec3 black{0.f, 0.f, 0.f};
    inline static const glm::vec3 cyan1{0.f, 1.f, 1.f};
    inline static const glm::vec3 aquamarine2{0.5f, 1.0f, 0.83f};
    inline static const glm::vec3 olive_drab1{0.753f, 1.0f, 0.243f};
    inline static const glm::vec3 rosy_brown{1.0f, 0.757f, 0.757f};
    inline static const glm::vec3 indian_red1{1.0f, 0.416f, 0.416f};
    inline static const glm::vec3 deep_sky_blue2{0.0f, 0.698f, 0.933f};
    inline static const glm::vec3 gray41{0.412f, 0.412f, 0.412f};


    /* 将 0-255 的整形颜色值转化为 0-1.f 的浮点颜色值  */
    inline static glm::vec3 color(uint8_t r, uint8_t g, uint8_t b) {
        return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
    }
};

#endif //RENDER_COLOR_H
