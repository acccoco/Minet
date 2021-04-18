#ifndef RENDER_COLOR_H
#define RENDER_COLOR_H

#include <glm/glm.hpp>


/**
 * 颜色常量
 */
class Color {
public:
    static glm::vec3 white;
    static glm::vec3 black;
    static glm::vec3 cyan1;
    static glm::vec3 aquamarine2;
    static glm::vec3 olive_drab1;
    static glm::vec3 rosy_brown;
    static glm::vec3 indian_red1;
    static glm::vec3 deep_sky_blue2;
    static glm::vec3 gray41;

    static glm::vec3 color(uint8_t r, uint8_t g, uint8_t b) {
        return glm::vec3(r/256.f, g/256.f, b/256.f);
    }
};

#endif //RENDER_COLOR_H
