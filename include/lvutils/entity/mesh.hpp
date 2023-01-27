#ifndef LV_MESH_H
#define LV_MESH_H

#include <array>
//#include <iterator>
//#include <map>
#include <string>

#include "backend.hpp"

#include "../libraries/glm.hpp"

#include "lvcore/core/buffer.hpp"
#include "lvcore/core/texture.hpp"
#ifdef LV_BACKEND_VULKAN
#include "lvcore/core/descriptor_set.hpp"
#endif
#include "vertex.hpp"

#define LV_MESH_TEXTURE_COUNT 2

namespace lv {

/*
struct UBOAvailableTextures {
  bool availableTextures[4] = {false};
};
*/

//TODO: destroy textures once at mesh destroying
class MeshComponent {
public:
    //std::vector<Texture*>& textures;
    //std::map<std::string, bool> setTextures;

    std::vector<MainVertex> vertices;
    std::vector<uint32_t> indices;

    std::string vertDataFilename;
    std::string indDataFilename;

    //Rendering
    Buffer vertexBuffer;
    Buffer indexBuffer;
    //UniformBuffer uniformBuffer = UniformBuffer(sizeof(UBOAvailableTextures));
    //UBOAvailableTextures uboAvailableTextures;

    static Texture neautralTexture;

    static std::vector<lv::Texture*> loadedTextures;

    std::array<Texture*, LV_MESH_TEXTURE_COUNT> textures = {
        &neautralTexture, &neautralTexture
    };

    //Dimensions
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float radius;

#ifdef LV_BACKEND_METAL
		static uint16_t bindingIndices[2];
#endif

#ifdef LV_BACKEND_VULKAN
    MeshComponent(PipelineLayout& pipelineLayout) { descriptorSet = new DescriptorSet(pipelineLayout, 2); }
#endif

    void init(std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices/*, std::vector<Texture*>& aTextures*/);

    void destroy();

#ifdef LV_BACKEND_VULKAN
    void initDescriptorSet();

    void destroyDescriptorSet();
#endif

    void setTexture(lv::Texture* texture, uint8_t index);

    void render();

    void renderShadows();

    void renderNoTextures(
#ifdef LV_BACKEND_METAL
        uint8_t bindingIndex
#endif
    );

	void createPlane();

    void loadFromFile(const char* aVertDataFilename, const char* aIndDataFilename);

    //Static functions
    static Texture* loadTextureFromFile(const char* filename);

private:
#ifdef LV_BACKEND_VULKAN
    DescriptorSet* descriptorSet;
#endif
};

} //namespace lv

#endif
