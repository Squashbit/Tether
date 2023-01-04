#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

#include <set>

using namespace Tether::Rendering::Vulkan;

static const std::vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Device::Device(
	Instance* instance,
	VkSurfaceKHR surface
)
	:
	instance(instance),
	iloader(instance->GetLoader()),
	surface(surface)
{
	TETHER_ASSERT(instance != nullptr);

	PickDevice();

	QueueFamilyIndices queueIndices = 
		instance->FindQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		queueIndices.graphicsFamilyIndex,
		queueIndices.presentFamilyIndex
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &features;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (instance->IsDebugMode())
	{
		// Enable validation layers

		createInfo.enabledLayerCount   = 
			static_cast<uint32_t>(instance->validationLayers.size());
		createInfo.ppEnabledLayerNames = instance->validationLayers.data();
	}
	
	// Create the device
	if (iloader->vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)
		!= VK_SUCCESS)
		throw RendererException("Device creation failed");

	loader.Load(iloader, &device);
}

Device::~Device()
{
	loader.vkDestroyDevice(device, nullptr);
}

VkQueue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
{
	VkQueue queue;
	loader.vkGetDeviceQueue(device, familyIndex, queueIndex, &queue);

	return queue;
}

void Device::WaitIdle()
{
	loader.vkDeviceWaitIdle(device);
}

VkDevice Device::Get()
{
	return device;
}

VkPhysicalDevice Device::GetPhysicalDevice()
{
	return physicalDevice;
}

DeviceLoader* Device::GetLoader()
{
	return &loader;
}

void Device::PickDevice()
{
	uint32_t deviceCount = 0;
	iloader->vkEnumeratePhysicalDevices(instance->Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		throw RendererException("No vulkan devices found");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	iloader->vkEnumeratePhysicalDevices(instance->Get(), &deviceCount,
		devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			return;
		}

	throw RendererException("No suitable device found");
}

bool Device::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	iloader->vkGetPhysicalDeviceProperties(device, &deviceProperties);
	iloader->vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices families = instance->FindQueueFamilies(device, surface);

	bool extentionsSupported = CheckDeviceExtentionSupport(device,
		deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		SwapchainDetails details = instance->QuerySwapchainSupport(device,
			surface);
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

bool Device::CheckDeviceExtentionSupport(VkPhysicalDevice device,
	const char* const* deviceExtentions, uint64_t extentionCount)
{
	// The device requires some extentions (such as the swap chain)
	// We need to check for those here before we can use the device.

	uint32_t count;
	iloader->vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
		nullptr);

	std::vector<VkExtensionProperties> availableExtentions(count);
	iloader->vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
		availableExtentions.data());

	std::vector<std::string> requiredExtentions(extentionCount);
	for (uint64_t i = 0; i < extentionCount; i++)
		requiredExtentions[i] = std::string(deviceExtentions[i]);

	std::set<std::string> requiredExtentionSet(requiredExtentions.begin(),
		requiredExtentions.end());

	for (size_t i = 0; i < availableExtentions.size(); i++)
		requiredExtentionSet.erase(availableExtentions[i].extensionName);

	return requiredExtentionSet.empty();
}
