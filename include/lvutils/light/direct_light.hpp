#ifndef LV_DIRECT_LIGHT_H
#define LV_DIRECT_LIGHT_H

#include "../libraries/glm.hpp"

#include "lvcore/core/uniform_buffer.hpp"

namespace lv {

struct UniformDirectLight {
    glm::vec4 color = glm::vec4(2.5f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
};

class DirectLight {
public:
    UniformDirectLight light;
    UniformBuffer lightUniformBuffer = UniformBuffer(sizeof(UniformDirectLight));

    DirectLight() = default;

    void destroy() { lightUniformBuffer.destroy(); }

    void uploadUniforms() { lightUniformBuffer.upload(&light); }
};

} //namespace lv

#endif
