#include <Tether/Renderer/Vulkan/VkUtils.hpp>
#include <vector>

using namespace Tether::Vulkan;

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool IsDeviceSuitable(VkPhysicalDevice device, Vulkan::Instance* pInstance,
	Vulkan::Surface* pSurface)
{
	InstanceLoader* iloader = pInstance->GetLoader();

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	iloader->vkGetPhysicalDeviceProperties(device, &deviceProperties);
	iloader->vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	Vulkan::QueueFamilyIndices families =
		pInstance->FindQueueFamilies(device, pSurface);

	bool extentionsSupported = pInstance->CheckDeviceExtentionSupport(device,
		deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		Vulkan::SwapchainDetails details = pInstance->QuerySwapchainSupport(device,
			pSurface);
		swapChainGood = !details.formats.empty()
			&& !details.presentModes.empty();
	}

	return
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& families.hasGraphicsFamily
		&& families.hasPresentFamily
		&& swapChainGood
		&& extentionsSupported;
}

bool Utils::PickDevice(VkPhysicalDevice* pDevice, Vulkan::Instance* pInstance, 
	Vulkan::Surface* pSurface)
{
	InstanceLoader* iloader = pInstance->GetLoader();

	uint32_t deviceCount = 0;
	iloader->vkEnumeratePhysicalDevices(pInstance->Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		return false;

	std::vector<VkPhysicalDevice> devices(deviceCount);
	iloader->vkEnumeratePhysicalDevices(pInstance->Get(), &deviceCount,
		devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device, pSurface))
		{
			*pDevice = device;
			return true;
		}

	return false;
}