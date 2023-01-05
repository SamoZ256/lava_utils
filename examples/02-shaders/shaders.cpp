#include <Core/SwapChain.hpp>
#include <Core/Allocator.hpp>
#include <Core/Descriptor.hpp>
#include <Core/Shader.hpp>
#include <Core/Viewport.hpp>

#include <Window/Window.hpp>

#define SHADER_COUNT 1

enum ShaderType {
    SHADER_TYPE_MAIN
};

int main() {
    lv::Window window(1920, 1080, "02-shaders");

    lv::InstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.applicationName = "Lava Engine Shaders Example";
	instanceCreateInfo.enableValidationLayers = true;
	instanceCreateInfo.enableStatisticsLayers = true;
	lv::Instance instance(instanceCreateInfo);
	
	lv::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.window = &window;
	lv::Device device(deviceCreateInfo);
	
	lv::AllocatorCreateInfo allocatorCreateInfo;
	lv::Allocator allocator(allocatorCreateInfo);
	
	lv::SwapChainCreateInfo swapChainCreateInfo;
	swapChainCreateInfo.window = &window;
	lv::SwapChain swapChain(swapChainCreateInfo);
	
	lv::DescriptorManagerCreateInfo descriptorManagerCreateInfo;
	descriptorManagerCreateInfo.pipelineLayoutCount = SHADER_COUNT;
	lv::DescriptorManager descriptorManager(descriptorManagerCreateInfo);

    //Shaders

	//Main shader
	descriptorManager.getPipelineLayout(SHADER_TYPE_MAIN).init();

	lv::ShaderModuleCreateInfo vertMainCreateInfo{};
	vertMainCreateInfo.shaderType = VK_SHADER_STAGE_VERTEX_BIT;
	vertMainCreateInfo.filename = "02-shaders/Shaders/Compiled/Vertex/main.vert.spv";

	lv::ShaderModule vertMainModule(vertMainCreateInfo);

	lv::ShaderModuleCreateInfo fragMainCreateInfo{};
	fragMainCreateInfo.shaderType = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragMainCreateInfo.filename = "02-shaders/Shaders/Compiled/Fragment/main.frag.spv";

	lv::ShaderModule fragMainModule(fragMainCreateInfo);

	lv::ShaderCreateInfo mainGraphicsPipelineCreateInfo{};
	
	mainGraphicsPipelineCreateInfo.vertexShaderModule = &vertMainModule;
	mainGraphicsPipelineCreateInfo.fragmentShaderModule = &fragMainModule;
	mainGraphicsPipelineCreateInfo.pipelineLayoutIndex = SHADER_TYPE_MAIN;
	mainGraphicsPipelineCreateInfo.renderPass = nullptr;

	lv::Shader mainGraphicsPipeline(mainGraphicsPipelineCreateInfo);

    //Viewports
    lv::Viewport viewport(0, 0, window.width, window.height);

    while (window.isOpen()) {
        window.pollEvents();

        if (window.resized) {
            device.waitIdle();

            swapChain.recreate(window);

            viewport.setViewport(0, 0, window.width, window.height);

            window.resized = false;
        }

        swapChain.acquireNextImage();

        swapChain.framebuffer.bind();

        mainGraphicsPipeline.bind();
        viewport.bind();

        swapChain.renderFullscreenTriangle();

        swapChain.framebuffer.unbind();

        swapChain.renderAndPresent();
    }

    device.waitIdle();

    vertMainModule.destroy();
    fragMainModule.destroy();
    mainGraphicsPipeline.destroy();

    descriptorManager.destroy();
    allocator.destroy();
    swapChain.destroy();
	device.destroy();
    instance.destroy();

    window.destroy();

    return 0;
}
