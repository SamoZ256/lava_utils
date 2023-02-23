#include "lvutils/entity/material.hpp"

#include <iostream>

namespace lv {

MaterialComponent::MaterialComponent(PipelineLayout& pipelineLayout) {
    materialUniformBuffer.usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    materialUniformBuffer.memoryType = LV_MEMORY_TYPE_SHARED;
    materialUniformBuffer.init(sizeof(UBOMaterial));

    descriptorSet.pipelineLayout = &pipelineLayout;
    descriptorSet.layoutIndex = 1;
    descriptorSet.addBinding(materialUniformBuffer.descriptorInfo(), 0);
    descriptorSet.init();
}

void MaterialComponent::uploadUniforms() {
    //std::cout << "MATERIAL: " << (int)descriptorSet.shaderType << std::endl;
    materialUniformBuffer.copyDataTo(0, &material);
    descriptorSet.bind();
}

void MaterialComponent::destroy() {
    materialUniformBuffer.destroy();
}

} //namespace lv
