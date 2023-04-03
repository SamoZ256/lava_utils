#include "lvutils/skylight/skylight.hpp"

#include <iostream>

#include <stb/stb_image.h>

#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/device.hpp"
#include "lvcore/core/viewport.hpp"
#include "lvcore/core/buffer.hpp"
#include "lvcore/core/framebuffer.hpp"
#include "lvcore/core/swap_chain.hpp"
#include "lvcore/core/command_buffer.hpp"

#include "lvutils/entity/mesh.hpp"

namespace lv {

Skylight::Skylight(uint8_t threadIndex) {
    vertexBuffer.frameCount = 1;
    vertexBuffer.usage = LV_BUFFER_USAGE_TRANSFER_DST_BIT | LV_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.init(vertices.size() * sizeof(Vertex3D));
    vertexBuffer.copyDataTo(threadIndex, vertices.data());

    indexBuffer.frameCount = 1;
    indexBuffer.usage = LV_BUFFER_USAGE_TRANSFER_DST_BIT | LV_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBuffer.init(indices.size() * sizeof(uint32_t));
    indexBuffer.copyDataTo(threadIndex, indices.data());
}

void Skylight::load(uint8_t threadIndex, const char* aFilename, GraphicsPipeline& equiToCubeGraphicsPipeline) {
    //Sampler
    sampler.filter = LV_FILTER_LINEAR;
    sampler.init();

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

    Texture loadedTexture;
    loadedTexture.init(filename.c_str());

    environmentMapImage.frameCount = 1;
    environmentMapImage.format = format;
    environmentMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
    environmentMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    environmentMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    environmentMapImage.layerCount = 6;
    environmentMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);
    
    Framebuffer framebuffer;
    framebuffer.frameCount = 1;
    
    CommandBuffer commandBuffer;
    commandBuffer.frameCount = 1;
    commandBuffer.init();

    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);

    framebuffer.addColorAttachment({
        .image = &environmentMapImage,
        .index = 0
    });

    framebuffer.init(equiToCubeGraphicsPipeline.renderPass);
    
    Buffer uniformBuffers[6];
    
    DescriptorSet descriptorSets[6];
    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].pipelineLayout = equiToCubeGraphicsPipeline.pipelineLayout;
        descriptorSets[i].layoutIndex = 0;

        uniformBuffers[i].usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffers[i].memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffers[i].init(sizeof(UBOCubemapVP));

        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(sampler.descriptorInfo(loadedTexture.image), 1);
        descriptorSets[i].init();
    }

    UBOCubemapVP uboCubemapVPs[6];

    commandBuffer.bind();

    framebuffer.bind();

    equiToCubeGraphicsPipeline.bind();
    viewport.bind();

    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].bind();
        
        uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
        uboCubemapVPs[i].layerIndex = (int)i;
        uniformBuffers[i].copyDataTo(0, &uboCubemapVPs[i]);

        g_swapChain->renderFullscreenTriangle();
    }

    framebuffer.unbind();

    commandBuffer.unbind();

    commandBuffer.submit();

    //Clean up
    g_device->waitIdle();

    loadedTexture.image.destroy();

    framebuffer.destroy();
    commandBuffer.destroy();
}

void Skylight::createIrradianceMap(uint8_t threadIndex, GraphicsPipeline& irradianceGraphicsPipeline) {
    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);

    irradianceMapImage.frameCount = 1;
    irradianceMapImage.format = format;
    irradianceMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
    irradianceMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    irradianceMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    irradianceMapImage.layerCount = 6;
    irradianceMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);

    Framebuffer framebuffer;
    framebuffer.frameCount = 1;
    
    CommandBuffer commandBuffer;
    commandBuffer.frameCount = 1;
    commandBuffer.init();

    framebuffer.addColorAttachment({
        .image = &irradianceMapImage,
        .index = 0
    });

    framebuffer.init(irradianceGraphicsPipeline.renderPass);
    
    Buffer uniformBuffers[6];
    
    DescriptorSet descriptorSets[6];
    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].pipelineLayout = irradianceGraphicsPipeline.pipelineLayout;
        descriptorSets[i].layoutIndex = 0;

        uniformBuffers[i].usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffers[i].memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffers[i].init(sizeof(UBOCubemapVP));

        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(sampler.descriptorInfo(environmentMapImage), 1);
        descriptorSets[i].init();
    }

    UBOCubemapVP uboCubemapVPs[6];

    commandBuffer.bind();
    
    framebuffer.bind();

    irradianceGraphicsPipeline.bind();
    viewport.bind();

    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].bind();
        
        uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
        uboCubemapVPs[i].layerIndex = (int)i;
        uniformBuffers[i].copyDataTo(0, &uboCubemapVPs[i]);

        g_swapChain->renderFullscreenTriangle();
    }

    framebuffer.unbind();

    commandBuffer.unbind();

    commandBuffer.submit();

    //Clean up
    g_device->waitIdle();

    framebuffer.destroy();
    commandBuffer.destroy();
}

void Skylight::createPrefilteredMap(uint8_t threadIndex, GraphicsPipeline& prefilteredGraphicsPipeline) {
    Viewport viewport(0, 0, SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);

    prefilteredMapImage.frameCount = 1;
    prefilteredMapImage.format = format;
    prefilteredMapImage.usage |= LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_SAMPLED_BIT;
    prefilteredMapImage.aspectMask = LV_IMAGE_ASPECT_COLOR_BIT;
    prefilteredMapImage.viewType = LV_IMAGE_VIEW_TYPE_CUBE;
    prefilteredMapImage.layerCount = 6;
    prefilteredMapImage.mipCount = MAX_CUBEMAP_MIP_LEVELS;
    prefilteredMapImage.init(SKYLIGHT_IMAGE_SIZE, SKYLIGHT_IMAGE_SIZE);

    Image prefilteredMapImageViewsForWriting[MAX_CUBEMAP_MIP_LEVELS];
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++)
        prefilteredMapImage.newImageView(prefilteredMapImageViewsForWriting[i], LV_IMAGE_VIEW_TYPE_CUBE, 0, 6, i, 1);

    //Sampler
    prefilteredMapSampler.filter = LV_FILTER_LINEAR;
    prefilteredMapSampler.maxLod = MAX_CUBEMAP_MIP_LEVELS - 1;
    prefilteredMapSampler.init();

    Framebuffer framebuffers[MAX_CUBEMAP_MIP_LEVELS];
    
    CommandBuffer commandBuffer;
    commandBuffer.frameCount = 1;
    commandBuffer.init();

    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        framebuffers[i].frameCount = 1;
        framebuffers[i].addColorAttachment({
            .image = &prefilteredMapImageViewsForWriting[i],
            .index = 0
        });
    }
    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++) {
        framebuffers[i].init(prefilteredGraphicsPipeline.renderPass);
    }
    
    Buffer uniformBuffers[6];
    
    DescriptorSet descriptorSets[6];
    for (uint8_t i = 0; i < 6; i++) {
        descriptorSets[i].pipelineLayout = prefilteredGraphicsPipeline.pipelineLayout;
        descriptorSets[i].layoutIndex = 0;

        uniformBuffers[i].usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffers[i].memoryType = LV_MEMORY_TYPE_SHARED;
        uniformBuffers[i].init(sizeof(UBOCubemapVP));

        descriptorSets[i].frameCount = 1;
        descriptorSets[i].addBinding(uniformBuffers[i].descriptorInfo(), 0);
        descriptorSets[i].addBinding(sampler.descriptorInfo(environmentMapImage), 1);
        descriptorSets[i].init();
    }

    UBOCubemapVP uboCubemapVPs[6];

    commandBuffer.bind();

    for (uint8_t mip = 0; mip < MAX_CUBEMAP_MIP_LEVELS; mip++) {
        framebuffers[mip].bind();

        prefilteredGraphicsPipeline.bind();
        uint32_t size = SKYLIGHT_IMAGE_SIZE / pow(2, mip);
        viewport.setViewport(0, 0, size, size);
        viewport.bind();

        float roughness = float(mip) / float(MAX_CUBEMAP_MIP_LEVELS - 1);
        prefilteredGraphicsPipeline.uploadPushConstants(&roughness, 0);
        for (uint8_t i = 0; i < 6; i++) {
            descriptorSets[i].bind();
            
            uboCubemapVPs[i].viewProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f) * captureViews[i];
            uboCubemapVPs[i].layerIndex = (int)i;
            uniformBuffers[i].copyDataTo(0, &uboCubemapVPs[i]);

            g_swapChain->renderFullscreenTriangle();
        }

        framebuffers[mip].unbind();
    }

    commandBuffer.unbind();

    commandBuffer.submit();

    //Clean up
    g_device->waitIdle();

    for (uint8_t i = 0; i < MAX_CUBEMAP_MIP_LEVELS; i++)
        framebuffers[i].destroy();
    commandBuffer.destroy();
}

void Skylight::destroy() {
    sampler.destroy();

    environmentMapImage.destroy();

    irradianceMapImage.destroy();

    prefilteredMapImage.destroy();
    prefilteredMapSampler.destroy();
}

} //namespace lv
