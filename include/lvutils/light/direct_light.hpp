#ifndef LV_DIRECT_LIGHT_H
#define LV_DIRECT_LIGHT_H

#include "../libraries/glm.hpp"

#include "lvcore/core/buffer.hpp"

namespace lv {

struct UniformDirectLight {
    glm::vec4 color = glm::vec4(4.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
};

class DirectLight {
public:
    UniformDirectLight light;
    Buffer lightUniformBuffer;

    DirectLight() {
        lightUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        lightUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
        lightUniformBuffer.init(sizeof(UniformDirectLight));
    }

    void destroy() { lightUniformBuffer.destroy(); }

    void uploadUniforms() { lightUniformBuffer.copyDataTo(0, &light); }
};

} //namespace lv

#endif
