#include <Tether/Renderer/Vulkan/DeviceLoader.hpp>
#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/NativeVulkan.hpp>

using namespace Tether::Vulkan;

void DeviceLoader::Load(InstanceLoader* iloader, VkDevice* pDevice)
{
	if (!iloader || !pDevice)
		return;

	TETHER_DEVICE_FUNC(DeviceWaitIdle);
	TETHER_DEVICE_FUNC(GetDeviceQueue);
}
