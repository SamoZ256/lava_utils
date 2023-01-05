#ifndef LV_MATERIAL_H
#define LV_MATERIAL_H

#include "backend.hpp"

#include "../libraries/glm.hpp"

#include "lvcore/core/uniform_buffer.hpp"
#ifdef LV_BACKEND_VULKAN
#include "lvcore/core/descriptor_set.hpp"
#endif

namespace lv {

struct UBOMaterial {
  glm::vec4 albedo = glm::vec4(1.0f);
  float roughness = 1.0f;
  float metallic = 1.0f;
};

class MaterialComponent {
public:
    UBOMaterial material;

    UniformBuffer materialUniformBuffer = UniformBuffer(sizeof(UBOMaterial));
#ifdef LV_BACKEND_VULKAN
    DescriptorSet* descriptorSet;// = DescriptorSet(0, 1);
#endif

    //MaterialComponent(const MaterialComponent &) {}
    //void operator=(const MaterialComponent &) {}
#ifdef LV_BACKEND_VULKAN
    MaterialComponent(PipelineLayout& pipelineLayout);
#elif defined LV_BACKEND_METAL
    MaterialComponent() {}
#endif

    void uploadUniforms();

    void destroy();
};

} //namespace lv

#endif
