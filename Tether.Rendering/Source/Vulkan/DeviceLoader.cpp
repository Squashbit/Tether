#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether::Rendering::Vulkan;

void DeviceLoader::Load(InstanceLoader* iloader, VkDevice* pDevice)
{
	if (!iloader || !pDevice)
		return;

	TETHER_DEVICE_FUNC(CreateImageView);
	TETHER_DEVICE_FUNC(CreateRenderPass);
	TETHER_DEVICE_FUNC(CreateSwapchainKHR);
	TETHER_DEVICE_FUNC(DestroyDevice);
	TETHER_DEVICE_FUNC(DestroyRenderPass);
	TETHER_DEVICE_FUNC(DestroySwapchainKHR);
	TETHER_DEVICE_FUNC(DeviceWaitIdle);
	TETHER_DEVICE_FUNC(GetDeviceQueue);
	TETHER_DEVICE_FUNC(GetSwapchainImagesKHR);
}
