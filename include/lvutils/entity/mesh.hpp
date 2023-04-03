#ifndef LV_MESH_H
#define LV_MESH_H

#include <array>
//#include <iterator>
//#include <map>
#include <string>

#include "../libraries/glm.hpp"

#include "lvcore/core/enums.hpp"

#include "lvcore/core/buffer.hpp"
#include "lvcore/core/image.hpp"
#include "lvcore/core/sampler.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/pipeline_layout.hpp"

#include "vertex.hpp"

#define LV_MESH_TEXTURE_COUNT 3

namespace lv {

struct Texture {
    std::string filename;
    Image image;
    Sampler sampler;

    void init(const char* aFilename, LvFormat format = LV_FORMAT_R8G8B8A8_UNORM, bool generateMipmaps = false) {
        filename = std::string(aFilename);
        image.frameCount = 1;
        image.format = format;
        image.usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_TRANSFER_DST_BIT | LV_IMAGE_USAGE_TRANSFER_SRC_BIT;
        image.initFromFile(aFilename, generateMipmaps);
        image.transitionLayout(0, 0, LV_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        sampler.filter = LV_FILTER_LINEAR;
        sampler.addressMode = LV_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler.maxLod = image.mipCount;
        sampler.init();
    }
};

//TODO: destroy textures once at mesh destroying
class MeshComponent {
public:
    std::vector<MainVertex> vertices;
    std::vector<uint32_t> indices;

    std::string vertDataFilename;
    std::string indDataFilename;

    //Rendering
    Buffer vertexBuffer;
    Buffer indexBuffer;

    static Texture neutralTexture;
    static Texture normalNeutralTexture;

    static std::vector<Texture*> loadedTextures;

    std::array<Texture*, LV_MESH_TEXTURE_COUNT> textures = {
        &neutralTexture, &neutralTexture, &normalNeutralTexture
    };

    //Dimensions
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float radius;

    MeshComponent(PipelineLayout& pipelineLayout) {
        descriptorSet.pipelineLayout = &pipelineLayout;
        descriptorSet.layoutIndex = 2;
    }

    void init(uint8_t threadIndex, std::vector<MainVertex>& aVertices, std::vector<uint32_t>& aIndices/*, std::vector<Texture*>& aTextures*/);

    void destroy();

    void initDescriptorSet();

    void destroyDescriptorSet();

    void setTexture(lv::Texture* texture, uint8_t index);

    void render();

    void renderShadows();

    void renderNoTextures();

	void createPlane(uint8_t threadIndex);

    void loadFromFile(uint8_t threadIndex, const char* aVertDataFilename, const char* aIndDataFilename);

    //Static functions
    static Texture* loadTextureFromFile(uint8_t threadIndex, const char* filename, LvFormat format = LV_FORMAT_R8G8B8A8_UNORM_SRGB);

private:
    DescriptorSet descriptorSet;
};

} //namespace lv

#endif
