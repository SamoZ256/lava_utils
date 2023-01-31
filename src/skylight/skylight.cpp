#include "lvutils/skylight/skylight.hpp"

#include <iostream>

#include <stb/stb_image.h>

#ifdef LV_BACKEND_VULKAN
#include "lvcore/core/allocator.hpp"
#include "lvcore/core/descriptor_set.hpp"
#endif
#include "lvcore/core/device.hpp"
#include "lvcore/core/viewport.hpp"
#include "lvcore/core/uniform_buffer.hpp"
#include "lvcore/core/framebuffer.hpp"
#include "lvcore/core/swap_chain.hpp"

namespace lv {

Skylight::Skylight() {
    vertexBuffer = new Buffer;
#ifdef LV_BACKEND_VULKAN
    vertexBuffer->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
#endif
    vertexBuffer->init(vertices.data(), vertices.size() * sizeof(Vertex3D));
    indexBuffer = new Buffer;
#ifdef LV_BACKEND_VULKAN
    indexBuffer->usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
#endif
    indexBuffer->init(indices.data(), indices.size() * sizeof(uint32_t));
}

void Skylight::load(const char* aFilename, GraphicsPipelineCreateInfo& equiToCubeGraphicsPipelineCreateInfo) {
    //Projection and view
    captureViews = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    filename = aFilename;
    int width, height, nbChannels;
    void* imageData = stbi_load(aFilename, &width, &height, &nbChannels, STBI_rgb_alpha);
    if (!imageData) {
        throw std::runtime_error(("Failed to load image '" + std::string(filename) + "'").c_str());
    }

#ifdef LV_BACKEND_VULKAN
    VkDeviceSize imageSize = width * height * 4;

    VkBuffer stagingBuffer;

    //VmaAllocationInfo allocInfo;
    VmaAllocation stagingAllocation = Vulkan_BufferHelper::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    void* data;
    vmaMapMemory(g_allocator->allocator, stagingAllocation, &data);
    memcpy(data, imageData, imageSize);
    vmaUnmapMemory(g_allocator->allocator, stagingAllocation);

    //Creating image
    //VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;//getImageFormat((uint8_t)nbChannels);
    VkImage stagingImage;
    VkImageView stagingImageView;
    VkSampler stagingSampler;

    VmaAllocation stagingImageAllocation = Vulkan_ImageHelper::createImage((uint16_t)width, (uint16_t)height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | LV_IMAGE_USAGE_SAMPLED_BIT, stagingImage, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    Vulkan_ImageHelper::transitionImageLayout(stagingImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Vulkan_BufferHelper::copyBufferToImage(stagingBuffer, stagingImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    Vulkan_ImageHelper::transitionImageLayout(stagingImage, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //Image view
    Vulkan_ImageHelper::createImageView(stagingImageView, stagingImage, format);

    //Sampler
    Vulkan_ImageHelper::createImageSampler(stagingSampler, LV_FILTER_LINEAR);

    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
#elif defined LV_BACKEND_METAL
    Image stagingImage;
    stagingImage.frameCount = 1;
    stagingImage.format = format;
    stagingImage.usage = LV_IMAGE_USAGE_SAMPLED_BIT;
    stagingImage.memoryProperties = LV_MEMORY_PROPERTY_SHARED;
    stagingImage.init(width, height);
    stagingImage.images[0]->replaceRegion(MTL::Region(0, 0, 0, width, height, 1), 0, imageData, width * 4);

    Sampler stagingSampler;
    stagingSampler.filter = LV_FILTER_LINEAR;
    stagingSampler.init();
#endif

    environmentMapImage.frameCount = 1;
    environmentMapImage.format = format;
    environmentMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
#ifdef LV_BACKEND_VULKAN
    environmentMapImage.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
#endif
    environmentMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    //environmentMapImage.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    environmentMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    environmentMapImage.layerCount = 6;
    //environmentMapImage.dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    environmentMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
    //environmentMapImage.transitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //allocation = ImageBuffer::createImage(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, image, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    //ImageBuffer::transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);

    //Image view
    environmentMapImageView.init(&environmentMapImage);
    //ImageBuffer::createImageView(imageView, image, format, VK_IMAGE_ASPECT_COLOR_BIT, 6, VK_IMAGE_VIEW_TYPE_CUBE);

    //Sampler
    environmentMapSampler.filter = LV_FILTER_LINEAR;
    environmentMapSampler.init();

#ifdef LV_BACKEND_VULKAN
    environmentMapImage.transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#endif
    //ImageBuffer::createImageSampler(sampler, VK_FILTER_LINEAR);

    //mainRenderPass.depthAttachment.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    //mainRenderPass.depthAttachment.init(mainRenderPass.framebuffer.width, mainRenderPass.framebuffer.height, lv::g_swapChain->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    
    Subpass subpass;
    RenderPass renderPass;
    Framebuffer framebuffer;
    framebuffer.frameCount = 1;

    /*
    Image tempImage;
    tempImage.format = format;
    tempImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tempImage.layerCount = 6;
    tempImage.images.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImage.images[i] = image;

    ImageView tempImageView;
    tempImageView.imageViews.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImageView.imageViews[i] = imageView;
    */

    framebuffer.addColorAttachment({
        .imageView = &environmentMapImageView,
        .index = 0
    });
    //mainRenderPass.framebuffer.setDepthAttachment(&mainRenderPass.depthAttachment, 1);

    //std::cout << "Test 1 passed" << std::endl;
    if (equiToCubeGraphicsPipeline == nullptr) {
        subpass.addColorAttachment({
            .index = 0,
            .layout = LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        });

        renderPass.addSubpass(&subpass);
        
        renderPass.addColorAttachment({
            .format = environmentMapImage.format,
            .index = 0,
            .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        renderPass.init();
        equiToCubeGraphicsPipelineCreateInfo.renderPass = &renderPass;
        equiToCubeGraphicsPipeline = new GraphicsPipeline(equiToCubeGraphicsPipelineCreateInfo);
    }

    framebuffer.init(&renderPass);
    
#ifdef LV_BACKEND_VULKAN
    UniformBuffer uniformBuffers[6] = { UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)) };
    
    //std::cout << (int)equiToCubeShader.pipelineLayoutIndex << std::endl;
    DescriptorSet descriptorSets[6] = { DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0), DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0), DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0), DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0), DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0), DescriptorSet(equiToCubeGraphicsPipeline->pipelineLayout, 0) };
    for (uint8_t i = 0; i < 6; i++) {
        Vulkan_ImageInfo stagingSamplerInfo;
        stagingSamplerInfo.infos.resize(1);
        //for (uint8_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
        stagingSamplerInfo.infos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        stagingSamplerInfo.infos[0].imageView = stagingImageView;
        stagingSamplerInfo.infos[0].sampler = stagingSampler;
        stagingSamplerInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        //}
        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(stagingSamplerInfo, 1);
        descriptorSets[i].init();
    }
#endif

    UBOCubemapVP uboCubemapVPs[6];

    framebuffer.bind();

    equiToCubeGraphicsPipeline->bind();
#ifdef LV_BACKEND_VULKAN
    viewport.bind();
#endif

    for (uint8_t i = 0; i < 6; i++) {
#ifdef LV_BACKEND_VULKAN
        descriptorSets[i].bind();
#elif defined LV_BACKEND_METAL
        stagingImage.bind(0);
        stagingSampler.bind(0);
#endif
        
        uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
        uboCubemapVPs[i].layerIndex = (int)i;
        //equiToCubeShader->uploadPushConstants(&uboEquiVP, 0);
#ifdef LV_BACKEND_VULKAN
        uniformBuffers[i].upload(&uboCubemapVPs[i]);
#elif defined LV_BACKEND_METAL
        equiToCubeGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_VERTEX_BIT);
        equiToCubeGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_FRAGMENT_BIT);
#endif

        g_swapChain->renderFullscreenTriangle();
    }

    framebuffer.unbind();

    framebuffer.render();
    
    //Buffer::copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);
    //ImageBuffer::transitionImageLayout(environmentMapImage.images[0], format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);
    //environmentMapImage.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //Clean up
#ifdef LV_BACKEND_VULKAN
    g_device->waitIdle();

    vmaDestroyBuffer(g_allocator->allocator, stagingBuffer, stagingAllocation);

    stbi_image_free(imageData);

    vkDestroySampler(g_device->device(), stagingSampler, nullptr);
    vkDestroyImageView(g_device->device(), stagingImageView, nullptr);
    vmaDestroyImage(g_allocator->allocator, stagingImage, stagingImageAllocation);

    renderPass.destroy();
#elif defined LV_BACKEND_METAL
    stagingImage.destroy();
    stagingSampler.destroy();
#endif
    framebuffer.destroy();
}

void Skylight::createIrradianceMap(GraphicsPipelineCreateInfo& irradianceGraphicsPipelineCreateInfo) {
#ifdef LV_BACKEND_VULKAN
    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
#endif

    irradianceMapImage.frameCount = 1;
    irradianceMapImage.format = format;
    irradianceMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
#ifdef LV_BACKEND_VULKAN
    irradianceMapImage.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
#endif
    irradianceMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    irradianceMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    irradianceMapImage.layerCount = 6;
    /*
#ifdef LV_BACKEND_VULKAN
    irradianceMapImage.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
#endif
    */
    //irradianceMapImage.dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    irradianceMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
    //irradianceMapImage.transitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //allocation = ImageBuffer::createImage(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, image, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    //ImageBuffer::transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);

    //Image view
    irradianceMapImageView.init(&irradianceMapImage);
    //ImageBuffer::createImageView(imageView, image, format, VK_IMAGE_ASPECT_COLOR_BIT, 6, VK_IMAGE_VIEW_TYPE_CUBE);

    //Sampler
    irradianceMapSampler.filter = LV_FILTER_LINEAR;
    irradianceMapSampler.init();
    //ImageBuffer::createImageSampler(sampler, VK_FILTER_LINEAR);

    //mainRenderPass.depthAttachment.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    //mainRenderPass.depthAttachment.init(mainRenderPass.framebuffer.width, mainRenderPass.framebuffer.height, lv::g_swapChain->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    Subpass subpass;
    RenderPass renderPass;
    Framebuffer framebuffer;
    framebuffer.frameCount = 1;

    /*
    Image tempImage;
    tempImage.format = format;
    tempImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tempImage.layerCount = 6;
    tempImage.images.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImage.images[i] = image;

    ImageView tempImageView;
    tempImageView.imageViews.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImageView.imageViews[i] = imageView;
    */

    framebuffer.addColorAttachment({
        .imageView = &irradianceMapImageView,
        .index = 0
    });
    //mainRenderPass.framebuffer.setDepthAttachment(&mainRenderPass.depthAttachment, 1);

    //std::cout << "Test 1 passed" << std::endl;
    if (irradianceGraphicsPipeline == nullptr) {
        subpass.addColorAttachment({
            .index = 0,
            .layout = LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        });

        renderPass.addSubpass(&subpass);
        
        renderPass.addColorAttachment({
            .format = irradianceMapImage.format,
            .index = 0,
            .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        renderPass.init();
        irradianceGraphicsPipelineCreateInfo.renderPass = &renderPass;
	    irradianceGraphicsPipeline = new GraphicsPipeline(irradianceGraphicsPipelineCreateInfo);
    }

    framebuffer.init(&renderPass);

#ifdef LV_BACKEND_VULKAN
    irradianceMapImage.transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    UniformBuffer uniformBuffers[6] = { UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)) };
    
    DescriptorSet descriptorSets[6] = { DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0), DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0), DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0), DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0), DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0), DescriptorSet(irradianceGraphicsPipeline->pipelineLayout, 0) };
    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(environmentMapSampler.descriptorInfo(environmentMapImageView), 1);
        descriptorSets[i].init();
    }
#endif

    UBOCubemapVP uboCubemapVPs[6];
    
    framebuffer.bind();

    irradianceGraphicsPipeline->bind();
#ifdef LV_BACKEND_VULKAN
    viewport.bind();
#endif

    for (uint8_t i = 0; i < 6; i++) {
#ifdef LV_BACKEND_VULKAN
        descriptorSets[i].bind();
#elif defined LV_BACKEND_METAL
        environmentMapImage.bind(0);
        environmentMapSampler.bind(0);
#endif
        
        uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
        uboCubemapVPs[i].layerIndex = (int)i;
        //irradianceShader->uploadPushConstants(&uboEquiVP, 0);
#ifdef LV_BACKEND_VULKAN
        uniformBuffers[i].upload(&uboCubemapVPs[i]);
#elif defined LV_BACKEND_METAL
        irradianceGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_VERTEX_BIT);
        irradianceGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_FRAGMENT_BIT);
#endif

        g_swapChain->renderFullscreenTriangle();
    }

    framebuffer.unbind();

    framebuffer.render();
    
    //Buffer::copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);
    //ImageBuffer::transitionImageLayout(irradianceMapImage.images[0], format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);
    //irradianceMapImage.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //Clean up
#ifdef LV_BACKEND_VULKAN
    g_device->waitIdle();
#endif

    renderPass.destroy();
    framebuffer.destroy();
}

void Skylight::createPrefilteredMap(GraphicsPipelineCreateInfo& prefilteredGraphicsPipelineCreateInfo) {
#ifdef LV_BACKEND_VULKAN
    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
#endif

    prefilteredMapImage.frameCount = 1;
    prefilteredMapImage.format = format;
    prefilteredMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
#ifdef LV_BACKEND_VULKAN
    prefilteredMapImage.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
#endif
    prefilteredMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    prefilteredMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    prefilteredMapImage.layerCount = 6;
    prefilteredMapImage.mipCount = MAX_CUBEMAP_MIP_LEVELS;
    /*
#ifdef LV_BACKEND_VULKAN
    prefilteredMapImage.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
#endif
    */
    //prefilteredMapImage.dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prefilteredMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
    //prefilteredMapImage.transitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //allocation = ImageBuffer::createImage(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, image, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    //ImageBuffer::transitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);

    //Image view
    prefilteredMapImageView.init(&prefilteredMapImage);
    //ImageBuffer::createImageView(imageView, image, format, VK_IMAGE_ASPECT_COLOR_BIT, 6, VK_IMAGE_VIEW_TYPE_CUBE);

    ImageView prefilteredMapImageViewsForWriting[MAX_CUBEMAP_MIP_LEVELS];
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        prefilteredMapImageViewsForWriting[i].viewType = LV_IMAGE_VIEW_TYPE_CUBE;
        prefilteredMapImageViewsForWriting[i].baseMip = i;
        prefilteredMapImageViewsForWriting[i].mipCount = 1;
        prefilteredMapImageViewsForWriting[i].init(&prefilteredMapImage);
    }
    /*
#elif defined LV_BACKEND_METAL
    Image prefilteredMapImagesForWriting[MAX_CUBEMAP_MIP_LEVELS];
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        prefilteredMapImagesForWriting[i] = prefilteredMapImage;
        prefilteredMapImagesForWriting[i].images[0] = prefilteredMapImage.images[0]->newTextureView(prefilteredMapImage.format, prefilteredMapImage.viewType, NS::Range(i, 1), NS::Range(0, 6));
    }
#endif
    */

    //Sampler
    prefilteredMapSampler.filter = LV_FILTER_LINEAR;
    prefilteredMapSampler.maxLod = MAX_CUBEMAP_MIP_LEVELS - 1;
    prefilteredMapSampler.init();
    //ImageBuffer::createImageSampler(sampler, VK_FILTER_LINEAR);

    //mainRenderPass.depthAttachment.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    //mainRenderPass.depthAttachment.init(mainRenderPass.framebuffer.width, mainRenderPass.framebuffer.height, lv::g_swapChain->depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    Subpass subpass;
    RenderPass renderPass;
    Framebuffer framebuffers[MAX_CUBEMAP_MIP_LEVELS];

    /*
    Image tempImage;
    tempImage.format = format;
    tempImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tempImage.layerCount = 6;
    tempImage.images.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImage.images[i] = image;

    ImageView tempImageView;
    tempImageView.imageViews.resize(2);
    for (uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) tempImageView.imageViews[i] = imageView;
    */

    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        framebuffers[i].frameCount = 1;
        framebuffers[i].addColorAttachment({
            .imageView = &prefilteredMapImageViewsForWriting[i],
            .index = 0
        });
    }
    //mainRenderPass.framebuffer.setDepthAttachment(&mainRenderPass.depthAttachment, 1);

    //std::cout << "Test 1 passed" << std::endl;
    if (prefilteredGraphicsPipeline == nullptr) {
        subpass.addColorAttachment({
            .index = 0,
            .layout = LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        });

        renderPass.addSubpass(&subpass);
        
        renderPass.addColorAttachment({
            .format = prefilteredMapImage.format,
            .index = 0,
            .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        renderPass.init();
        prefilteredGraphicsPipelineCreateInfo.renderPass = &renderPass;
	    prefilteredGraphicsPipeline = new GraphicsPipeline(prefilteredGraphicsPipelineCreateInfo);
    }
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        framebuffers[i].init(&renderPass);
        //framebuffers[i].clearValues[0].color = {float(i) / (MAX_CUBEMAP_MIP_LEVELS - 1), float(i) / (MAX_CUBEMAP_MIP_LEVELS - 1), float(i) / (MAX_CUBEMAP_MIP_LEVELS - 1), 1.0f};
    }

#ifdef LV_BACKEND_VULKAN
    prefilteredMapImage.transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    UniformBuffer uniformBuffers[6] = { UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)), UniformBuffer(sizeof(UBOCubemapVP)) };
    
    DescriptorSet descriptorSets[6] = { DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0), DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0), DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0), DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0), DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0), DescriptorSet(prefilteredGraphicsPipeline->pipelineLayout, 0) };
    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(environmentMapSampler.descriptorInfo(environmentMapImageView), 1);
        descriptorSets[i].init();
    }
#endif

    UBOCubemapVP uboCubemapVPs[6];

    for (uint8_t mip = 0; mip < MAX_CUBEMAP_MIP_LEVELS; mip++) {
        framebuffers[mip].bind();

        prefilteredGraphicsPipeline->bind();
#ifdef LV_BACKEND_VULKAN
        viewport.setViewport(0, 0, framebuffers[mip].width, framebuffers[mip].height);
        viewport.bind();
#endif

        float roughness = float(/*MAX_CUBEMAP_MIP_LEVELS - 1 - */mip) / float(MAX_CUBEMAP_MIP_LEVELS - 1);
        //std::cout << roughness << std::endl;
#ifdef LV_BACKEND_VULKAN
        prefilteredGraphicsPipeline->uploadPushConstants(&roughness, 0);
#elif defined LV_BACKEND_METAL
        prefilteredGraphicsPipeline->uploadPushConstants(&roughness, 1, sizeof(float), LV_SHADER_STAGE_FRAGMENT_BIT);
#endif
        for (uint8_t i = 0; i < 6; i++) {
#ifdef LV_BACKEND_VULKAN
            descriptorSets[i].bind();
#elif defined LV_BACKEND_METAL
            environmentMapImage.bind(0);
            environmentMapSampler.bind(0);
#endif
            
            uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
            uboCubemapVPs[i].layerIndex = (int)i;
#ifdef LV_BACKEND_VULKAN
            uniformBuffers[i].upload(&uboCubemapVPs[i]);
#elif defined LV_BACKEND_METAL
            prefilteredGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_VERTEX_BIT);
            prefilteredGraphicsPipeline->uploadPushConstants(&uboCubemapVPs[i], 0, sizeof(UBOCubemapVP), LV_SHADER_STAGE_FRAGMENT_BIT);
#endif

            g_swapChain->renderFullscreenTriangle();
        }

        framebuffers[mip].unbind();

        framebuffers[mip].render();
    }
    
    //Buffer::copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);
    //ImageBuffer::transitionImageLayout(prefilteredMapImage.images[0], format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6, MAX_CUBEMAP_MIP_LEVELS);
    //prefilteredMapImage.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //Clean up
#ifdef LV_BACKEND_VULKAN
    g_device->waitIdle();
#endif

    renderPass.destroy();
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++)
        framebuffers[i].destroy();
}

void Skylight::destroy() {
    /*
    vkDestroySampler(g_device->device(), sampler, nullptr);
    vkDestroyImageView(g_device->device(), imageView, nullptr);
    vmaDestroyImage(g_allocator->allocator, image, allocation);
    */
    environmentMapImage.destroy();
    environmentMapImageView.destroy();
    environmentMapSampler.destroy();

    irradianceMapImage.destroy();
    irradianceMapImageView.destroy();
    irradianceMapSampler.destroy();

    prefilteredMapImage.destroy();
    prefilteredMapImageView.destroy();
    prefilteredMapSampler.destroy();

    if (equiToCubeGraphicsPipeline != nullptr)
        equiToCubeGraphicsPipeline->destroy();
    if (irradianceGraphicsPipeline != nullptr)
        irradianceGraphicsPipeline->destroy();
    if (prefilteredGraphicsPipeline != nullptr)
        prefilteredGraphicsPipeline->destroy();
}

} //namespace lv
