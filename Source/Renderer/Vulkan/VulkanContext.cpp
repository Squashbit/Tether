#include <Tether/Renderer/Vulkan/VulkanContext.hpp>

#include <set>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Renderer/Vulkan/VkUtils.hpp>

#include <Tether/NativeVulkan.hpp>

using namespace Tether::Vulkan;

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanContext::ErrorCode VulkanContext::Init(ContextOptions* pOptions)
{
	Application& app = Application::Get();
	if (!app.IsInitialized() && !app.Init())
		return ErrorCode::APP_INIT_FAILED;
	if (!app.IsVulkanLoaded() && !app.LoadVulkan())
		return ErrorCode::APP_INIT_FAILED;

	instance = &app.GetVulkanNative()->instance;
	iloader = instance->GetLoader();

	if (!surface.Init(instance, pOptions->pWindow))
		return ErrorCode::SURFACE_INIT_FAILED;

	if (!VkUtils::PickDevice(&physicalDevice, instance, &surface))
		return ErrorCode::DEVICE_NOT_FOUND;

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);

	if (!InitDevice())
		return ErrorCode::DEVICE_INIT_FAILED;

	return ErrorCode::SUCCESS;
}

bool VulkanContext::InitDevice()
{
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

	if (!device.Init(
		instance,
		physicalDevice,
		queueCreateInfos.data(),
		queueCreateInfos.size(),
		features,
		deviceExtensions.data(),
		deviceExtensions.size()
	))
		return false;

	graphicsQueue = device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
	presentQueue = device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);

	return true;
}

void VulkanContext::OnDispose()
{
	device.Dispose();
	surface.Dispose();
}