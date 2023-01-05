#include <Core/SwapChain.hpp>
#include <Core/Allocator.hpp>
#include <Core/Descriptor.hpp>

#include <Window/Window.hpp>

int main() {
    lv::Window window(1920, 1080, "01-window");

    lv::InstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.applicationName = "Lava Engine Window Example";
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
	descriptorManagerCreateInfo.pipelineLayoutCount = 0;
	lv::DescriptorManager descriptorManager(descriptorManagerCreateInfo);

    swapChain.framebuffer.clearValues[0].color = {0.2f, 0.4f, 1.0f, 1.0f};

    while (window.isOpen()) {
        window.pollEvents();

        if (window.resized) {
            device.waitIdle();

            swapChain.recreate(window);

            swapChain.framebuffer.clearValues[0].color = {0.2f, 0.4f, 1.0f, 1.0f};

            window.resized = false;
        }

        swapChain.acquireNextImage();

        swapChain.framebuffer.bind();

        swapChain.framebuffer.unbind();

        swapChain.renderAndPresent();
    }

    device.waitIdle();

    descriptorManager.destroy();
    allocator.destroy();
    swapChain.destroy();
	device.destroy();
    instance.destroy();

    window.destroy();

    return 0;
}
