#ifndef LV_MATERIAL_H
#define LV_MATERIAL_H

#include "../libraries/glm.hpp"

#include "lvcore/core/buffer.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/pipeline_layout.hpp"

namespace lv {

struct UBOMaterial {
    glm::vec4 albedo = glm::vec4(1.0f);
    float roughness = 1.0f;
    float metallic = 1.0f;
};

class MaterialComponent {
public:
    UBOMaterial material;

    Buffer materialUniformBuffer;// = UniformBuffer(sizeof(UBOMaterial));
    DescriptorSet descriptorSet;

    //MaterialComponent(const MaterialComponent &) {}
    //void operator=(const MaterialComponent &) {}

    MaterialComponent(PipelineLayout& pipelineLayout);

    void uploadUniforms();

    void destroy();
};

} //namespace lv

#endif
