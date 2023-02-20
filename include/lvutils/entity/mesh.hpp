#ifndef LV_MESH_H
#define LV_MESH_H

#include <array>
//#include <iterator>
//#include <map>
#include <string>

#include "../libraries/glm.hpp"

#include "lvcore/core/enums.hpp"

#include "lvcore/core/buffer.hpp"
#include "lvcore/core/texture.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/pipeline_layout.hpp"

#include "vertex.hpp"

#define LV_MESH_TEXTURE_COUNT 3

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

    static Texture neutralTexture;
    static Texture normalNeutralTexture;

    static std::vector<lv::Texture*> loadedTextures;

    std::array<Texture*, LV_MESH_TEXTURE_COUNT> textures = {
        &neutralTexture, &neutralTexture, &normalNeutralTexture
    };

    //Dimensions
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float radius;

#ifdef LV_BACKEND_METAL
		static uint16_t bindingIndices[LV_MESH_TEXTURE_COUNT];
#endif

#ifdef LV_BACKEND_VULKAN
    MeshComponent(PipelineLayout& pipelineLayout) { descriptorSet = new DescriptorSet(pipelineLayout, 2); }
#endif

    void init(uint8_t threadIndex, std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices/*, std::vector<Texture*>& aTextures*/);

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

	void createPlane(uint8_t threadIndex);

    void loadFromFile(uint8_t threadIndex, const char* aVertDataFilename, const char* aIndDataFilename);

    //Static functions
    static Texture* loadTextureFromFile(uint8_t threadIndex, const char* filename, LvFormat format = LV_FORMAT_R8G8B8A8_UNORM_SRGB);

private:
#ifdef LV_BACKEND_VULKAN
    DescriptorSet* descriptorSet;
#endif
};

} //namespace lv

#endif
