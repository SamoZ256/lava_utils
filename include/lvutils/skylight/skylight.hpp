#ifndef LV_SKYLIGHT_H
#define LV_SKYLIGHT_H

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include "backend.hpp"

#include "lvcore/core/common.hpp"
#include "lvcore/core/enums.hpp"

#ifdef LV_BACKEND_VULKAN
#include "lvcore/core/image_view.hpp"
#endif
#include "lvcore/core/buffer.hpp"
#include "lvcore/core/graphics_pipeline.hpp"
#include "lvcore/core/sampler.hpp"

#include "../entity/vertex.hpp"

#define SKYLIGHT_IMAGE_SIZE 512
#define MAX_CUBEMAP_MIP_LEVELS 5

namespace lv {

struct UBOCubemapVP {
    glm::mat4 viewProj;
    int layerIndex;
};

class Skylight {
public:
    Image environmentMapImage;
#ifdef LV_BACKEND_VULKAN
    ImageView environmentMapImageView;
#endif
    Sampler environmentMapSampler;
    //VmaAllocation allocation;

    Image irradianceMapImage;
#ifdef LV_BACKEND_VULKAN
    ImageView irradianceMapImageView;
#endif
    Sampler irradianceMapSampler;

    Image prefilteredMapImage;
#ifdef LV_BACKEND_VULKAN
    ImageView prefilteredMapImageView;
#endif
    Sampler prefilteredMapSampler;
    
#ifdef LV_BACKEND_VULKAN
    VkFormat format = LV_FORMAT_RGBA8_UNORM;
#elif defined LV_BACKEND_METAL
    MTL::PixelFormat format = LV_FORMAT_RGBA8_UNORM;
#endif

    std::string filename;

    GraphicsPipeline* equiToCubeGraphicsPipeline = nullptr;
    GraphicsPipeline* irradianceGraphicsPipeline = nullptr;
    GraphicsPipeline* prefilteredGraphicsPipeline = nullptr;

    Buffer* vertexBuffer = nullptr;
    Buffer* indexBuffer = nullptr;

    std::vector<Vertex3D> vertices = {
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},

        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f,  1.0f, -1.0f}},
        {{-1.0f,  1.0f,  1.0f}},

        {{ 1.0f, -1.0f, -1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},

        {{-1.0f, -1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},

        {{-1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f, -1.0f}},
        {{ 1.0f,  1.0f,  1.0f}},
        {{-1.0f,  1.0f,  1.0f}},

        {{-1.0f, -1.0f, -1.0f}},
        {{-1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f,  1.0f}},
        {{ 1.0f, -1.0f, -1.0f}}
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23
    };

    //Projection and view
    //Right, left, top, bottom, front, back
    std::vector<glm::mat4> captureViews;

    Skylight();

    void load(const char* aFilename, GraphicsPipelineCreateInfo& equiToCubeGraphicsPipelineCreateInfo);

    void createIrradianceMap(GraphicsPipelineCreateInfo& irradianceGraphicsPipelineCreateInfo);

    void createPrefilteredMap(GraphicsPipelineCreateInfo& prefilteredGraphicsPipelineCreateInfo);

    void destroy();

    /*
    ImageInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
        ImageInfo info;
        info.infos.resize(1);
        //for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        info.infos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.infos[0].imageView = environmentMapImageView.imageViews[0];
        info.infos[0].sampler = environmentMapSampler.sampler;
        //}

        return info;
    }
    */
};

} //namespace lv

#endif
