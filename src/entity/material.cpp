#include "lvutils/entity/material.hpp"

#include <iostream>

namespace lv {

#ifdef LV_BACKEND_VULKAN
MaterialComponent::MaterialComponent(PipelineLayout& pipelineLayout) {
    descriptorSet = new DescriptorSet(pipelineLayout, 1);
    descriptorSet->addBinding(materialUniformBuffer.descriptorInfo(), 0);
    descriptorSet->init();
}
#endif

void MaterialComponent::uploadUniforms() {
    //std::cout << "MATERIAL: " << (int)descriptorSet.shaderType << std::endl;
    materialUniformBuffer.upload(&material);
#ifdef LV_BACKEND_VULKAN
    descriptorSet->bind();
#elif defined LV_BACKEND_METAL
    materialUniformBuffer.bindToFragmentShader(0);
#endif
}

void MaterialComponent::destroy() {
    materialUniformBuffer.destroy();
}

} //namespace lv
