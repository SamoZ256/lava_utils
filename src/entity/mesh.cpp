#include "lvutils/entity/mesh.hpp"

#include "lvcore/core/enums.hpp"

#include <iostream>

namespace lv {

Texture MeshComponent::neautralTexture = Texture();
std::vector<Texture*> MeshComponent::loadedTextures = std::vector<Texture*>();

#ifdef LV_BACKEND_METAL
uint16_t MeshComponent::bindingIndices[3] = {
    1, 0, 2
};
#endif

void MeshComponent::init(std::vector<MainVertex>& aVertices, std::vector<unsigned int>& aIndices) {
    vertices = aVertices;
    indices = aIndices;

    //std::cout << "MESH DESCRIPTOR_SET: " << (int)shaderType << std::endl;
#ifdef LV_BACKEND_VULKAN
    vertexBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    indexBuffer.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
#endif
    vertexBuffer.init(vertices.data(), vertices.size() * sizeof(MainVertex));
    indexBuffer.init(indices.data(), indices.size() * sizeof(uint32_t));
    /*
    for (uint8_t i = 0; i < textures.size(); i++) {
        if (textures[i] != nullptr)
        descriptorSet.addImageBinding(textures[i]->sampler.descriptorInfo(), i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    }
    */
    //descriptorSet.addBufferBinding(uniformBuffer.descriptorInfo(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

void MeshComponent::destroy() {
  vertexBuffer.destroy();
  indexBuffer.destroy();
  //descriptorSet.destroy();
  //for (auto& tex : textures) tex.destroy();
}

void MeshComponent::addTexture(lv::Texture* texture, uint16_t index) {
#ifdef LV_BACKEND_VULKAN
  if (texture == nullptr)
    descriptorSet->addImageBinding(neautralTexture.sampler.descriptorInfo(neautralTexture.imageView), index, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  else
    descriptorSet->addImageBinding(texture->sampler.descriptorInfo(texture->imageView), index, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
#elif defined LV_BACKEND_METAL
  if (texture == nullptr)
    textures.push_back({&neautralTexture, bindingIndices[index]});
  else
    textures.push_back({texture, bindingIndices[index]});
#endif
  if (texture != nullptr)
    texturesToSave.push_back({texture, index});
  //uboAvailableTextures.availableTextures[index] = true;
}

void MeshComponent::render() {
#ifdef LV_BACKEND_VULKAN
		descriptorSet->bind();
#elif defined LV_BACKEND_METAL
		for (auto& texturePair : textures) {
			texturePair.first->image.bind(texturePair.second);
			texturePair.first->sampler.bind(texturePair.second);
		}
#endif
    renderNoTextures(
#ifdef LV_BACKEND_METAL
        MainVertex::BINDING_INDEX
#endif
    );
}

void MeshComponent::renderShadows() {
    renderNoTextures(
#ifdef LV_BACKEND_METAL
        MainVertex::BINDING_INDEX_SHADOWS
#endif
    );
}

void MeshComponent::renderNoTextures(
#ifdef LV_BACKEND_METAL
uint8_t bindingIndex
#endif
) {
#ifdef LV_BACKEND_VULKAN
    vertexBuffer.bindVertexBuffer();
    indexBuffer.bindIndexBuffer(LV_INDEX_TYPE_UINT32);
    indexBuffer.renderIndexed(sizeof(uint32_t));
#elif defined LV_BACKEND_METAL
    vertexBuffer.bindVertexBuffer(bindingIndex);
    indexBuffer.renderIndexed(LV_INDEX_TYPE_UINT32, sizeof(uint32_t));
#endif
}

void MeshComponent::createPlane() {
    static std::vector<MainVertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}
    };
    static std::vector<uint32_t> indices = {
        0, 2, 1,
        0, 3, 2
    };

    init(vertices, indices);
}

Texture* MeshComponent::loadTextureFromFile(const char* filename) {
    std::string strFilename(filename);

    //Check if it has not been loaded yet
	for (unsigned int j = 0; j < loadedTextures.size(); j++) {
		if (loadedTextures[j]->filename == strFilename) {
			return loadedTextures[j];
		}
	}
	//}

	Texture* texture = new Texture;
	texture->load(filename);
	texture->init();
	loadedTextures.push_back(texture);

	return texture;
}

} //namespace lv
