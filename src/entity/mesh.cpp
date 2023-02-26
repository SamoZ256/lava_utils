#include "lvutils/entity/mesh.hpp"

#include "lvcore/core/enums.hpp"

#include "lvutils/disk_io/disk_io.hpp"

namespace lv {

Sampler MeshComponent::sampler = Sampler();
Texture MeshComponent::neutralTexture = Texture();
Texture MeshComponent::normalNeutralTexture = Texture();
std::vector<Texture*> MeshComponent::loadedTextures = std::vector<Texture*>();

void MeshComponent::init(uint8_t threadIndex, std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices) {
    vertices = aVertices;
    indices = aIndices;

    vertexBuffer.frameCount = 1;
    vertexBuffer.usage = LV_BUFFER_USAGE_TRANSFER_DST_BIT | LV_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.init(vertices.size() * sizeof(MainVertex));
    vertexBuffer.copyDataTo(threadIndex, vertices.data());

    indexBuffer.frameCount = 1;
    indexBuffer.usage = LV_BUFFER_USAGE_TRANSFER_DST_BIT | LV_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBuffer.init(indices.size() * sizeof(uint32_t));
    indexBuffer.copyDataTo(threadIndex, indices.data());

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

void MeshComponent::initDescriptorSet() {
    for (uint8_t i = 0; i < LV_MESH_TEXTURE_COUNT; i++) {
        descriptorSet.addBinding(sampler.descriptorInfo(textures[i]->imageView), i);
    }

    descriptorSet.init();
}

void MeshComponent::destroyDescriptorSet() {
    descriptorSet.destroy();
}

void MeshComponent::setTexture(Texture* texture, uint8_t index) {
    if (texture != nullptr)
        textures[index] = texture;
}

void MeshComponent::render() {
    descriptorSet.bind();
    renderNoTextures();
}

void MeshComponent::renderShadows() {
    renderNoTextures();
}

void MeshComponent::renderNoTextures() {
    vertexBuffer.bindVertexBuffer();
    indexBuffer.bindIndexBuffer(LV_INDEX_TYPE_UINT32);
    indexBuffer.renderIndexed(sizeof(uint32_t));
}

void MeshComponent::createPlane(uint8_t threadIndex) {
    static std::vector<MainVertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}
    };
    static std::vector<uint32_t> indices = {
        0, 2, 1,
        0, 3, 2
    };

    init(threadIndex, vertices, indices);
}

void MeshComponent::loadFromFile(uint8_t threadIndex, const char* aVertDataFilename, const char* aIndDataFilename) {
    vertDataFilename = std::string(aVertDataFilename);
    indDataFilename = std::string(aIndDataFilename);

    void* vertData;
    size_t vertSize;
    lv::loadRawBinary(aVertDataFilename, &vertData, &vertSize);

    std::vector<lv::MainVertex> vertices((lv::MainVertex*)vertData, (lv::MainVertex*)vertData + vertSize / sizeof(lv::MainVertex));

    void* indData;
    size_t indSize;
    lv::loadRawBinary(aIndDataFilename, &indData, &indSize);

    std::vector<uint32_t> indices((uint32_t*)indData, (uint32_t*)indData + indSize / sizeof(uint32_t));

    init(threadIndex, vertices, indices);
}

Texture* MeshComponent::loadTextureFromFile(uint8_t threadIndex, const char* filename, LvFormat format) {
    std::string strFilename(filename);

    //Check if it has not been loaded yet
	for (unsigned int j = 0; j < loadedTextures.size(); j++) {
		if (loadedTextures[j]->filename == strFilename) {
			return loadedTextures[j];
		}
	}

	Texture* texture = new Texture;
    texture->filename = strFilename;
    texture->init(filename, format, true);

	loadedTextures.push_back(texture);

	return texture;
}

} //namespace lv
