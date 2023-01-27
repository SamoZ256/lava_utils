#include "lvutils/entity/mesh.hpp"

#include "lvcore/core/enums.hpp"

#include "lvutils/disk_io/disk_io.hpp"

namespace lv {

Texture MeshComponent::neautralTexture = Texture();
std::vector<Texture*> MeshComponent::loadedTextures = std::vector<Texture*>();

#ifdef LV_BACKEND_METAL
uint16_t MeshComponent::bindingIndices[2] = {
    1, 0
};
#endif

void MeshComponent::init(std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices) {
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

    //Get dimensions
    float radius2 = 0.0f;
    for (auto& vertex : vertices) {
        minX = std::min(minX, vertex.position.x);
        minY = std::min(minY, vertex.position.y);
        minZ = std::min(minZ, vertex.position.z);

        maxX = std::max(maxX, vertex.position.x);
        maxY = std::max(maxY, vertex.position.y);
        maxZ = std::max(maxZ, vertex.position.z);

        radius2 = std::max(radius2, vertex.position.x * vertex.position.x + 
                                    vertex.position.y * vertex.position.y + 
                                    vertex.position.z * vertex.position.z);
    }
    radius = sqrt(radius2);
}

void MeshComponent::destroy() {
  vertexBuffer.destroy();
  indexBuffer.destroy();
  //descriptorSet.destroy();
  //for (auto& tex : textures) tex.destroy();
}

#ifdef LV_BACKEND_VULKAN
void MeshComponent::initDescriptorSet() {
    for (uint8_t i = 0; i < LV_MESH_TEXTURE_COUNT; i++) {
        descriptorSet->addBinding(textures[i]->sampler.descriptorInfo(textures[i]->imageView), i);
    }

    descriptorSet->init();
}

void MeshComponent::destroyDescriptorSet() {
    descriptorSet->destroy();
}
#endif

void MeshComponent::setTexture(Texture* texture, uint8_t index) {
    textures[index] = texture;
}

void MeshComponent::render() {
#ifdef LV_BACKEND_VULKAN
		descriptorSet->bind();
#elif defined LV_BACKEND_METAL
		for (uint8_t i = 0; i < LV_MESH_TEXTURE_COUNT; i++) {
			textures[i]->image.bind(bindingIndices[i]);
			textures[i]->sampler.bind(bindingIndices[i]);
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

void MeshComponent::loadFromFile(const char* aVertDataFilename, const char* aIndDataFilename) {
    vertDataFilename = std::string(aVertDataFilename);
    indDataFilename = std::string(aIndDataFilename);

    void* vertData;
    size_t vertSize;
    lv::loadRawBinary(aVertDataFilename, &vertData, &vertSize);

    std::vector<lv::MainVertex> vertices((lv::MainVertex*)vertData, (lv::MainVertex*)vertData + vertSize / sizeof(lv::MainVertex));//((std::istreambuf_iterator<char>(vertFile)), std::istreambuf_iterator<char>());

    void* indData;
    size_t indSize;
    lv::loadRawBinary(aIndDataFilename, &indData, &indSize);

    std::vector<uint32_t> indices((uint32_t*)indData, (uint32_t*)indData + indSize / sizeof(uint32_t));//((unsigned int*)indChar, (unsigned int*)indChar + strlen(indChar) * sizeof(char));

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
    texture->generateMipmaps = true;
	texture->init();
	loadedTextures.push_back(texture);

	return texture;
}

} //namespace lv
