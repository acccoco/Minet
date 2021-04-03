/**
 * 配置信息
 */

#ifndef RENDER_CONFIG_HPP
#define RENDER_CONFIG_HPP

#include <string>

// 路径的配置
// ============================================================================

#define ROOT_DIR "../"
#define ROOT(path) ROOT_DIR path

#define SHADER_DIR ROOT("assets/shader/")
#define SHADER(path) SHADER_DIR path

#define TEXTURE_DIR ROOT("assets/texture/")
#define TEXTURE(path) TEXTURE_DIR path


#endif //RENDER_CONFIG_HPP
