#ifndef LV_MESH_H
#define LV_MESH_H

#include <vector>
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
#ifdef LV_BACKEND_METAL
    std::vector<std::pair<Texture*, uint16_t> > textures;
#endif

    std::vector<std::pair<Texture*, uint16_t> > texturesToSave;
    std::vector<MainVertex> vertices;
    std::vector<unsigned int> indices;

    //Rendering
    Buffer vertexBuffer;
    Buffer indexBuffer;
#ifdef LV_BACKEND_VULKAN
    DescriptorSet* descriptorSet;// = DescriptorSet(0, 2);
#endif
    //UniformBuffer uniformBuffer = UniformBuffer(sizeof(UBOAvailableTextures));
    //UBOAvailableTextures uboAvailableTextures;

    static Texture neautralTexture;

    static std::vector<lv::Texture*> loadedTextures;

#ifdef LV_BACKEND_METAL
		static uint16_t bindingIndices[2];
#endif

#ifdef LV_BACKEND_VULKAN
    MeshComponent(PipelineLayout& pipelineLayout) { descriptorSet = new DescriptorSet(pipelineLayout, 2); }
#endif

    void init(std::vector<MainVertex>& aVertices, std::vector<unsigned int>& aIndices/*, std::vector<Texture*>& aTextures*/);

    void destroy();

    void addTexture(lv::Texture* texture, uint16_t index);

    void render();

    void renderShadows();

    void renderNoTextures(
#ifdef LV_BACKEND_METAL
        uint8_t bindingIndex
#endif
    );

	void createPlane();

    //Static functions
    static Texture* loadTextureFromFile(const char* filename);
};

} //namespace lv

#endif
