#include <Tether/Module/Rendering/Vulkan/SimpleNative.hpp>
#include <set>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/VkUtils.hpp>
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether::Rendering::Vulkan;

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

ErrorCode SimpleNative::Init(SimpleWindow* pWindow)
{
	Application& app = Application::Get();
	if (!app.IsInitialized() && !app.Init())
		return ErrorCode::APP_INIT_FAILED;

	RenderingModule& module = RenderingModule::Get();
	if (!module.IsVulkanInitialized())
		return ErrorCode::APP_INIT_FAILED;
	
	this->pWindow = pWindow;
	this->instance = &(RenderingModule::Get().GetVulkanNative()->instance);
	this->iloader = instance->GetLoader();

	if (!surface.Init(instance, pWindow))
		return ErrorCode::SURFACE_INIT_FAILED;

	if (!VkUtils::PickDevice(&physicalDevice, instance, &surface))
		return ErrorCode::DEVICE_NOT_FOUND;

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);

	if (!InitDevice())
		return ErrorCode::DEVICE_INIT_FAILED;
	if (!InitSwapchain())
		return ErrorCode::SWAPCHAIN_INIT_FAILED;
	if (!InitRenderPass())
		return ErrorCode::RENDERPASS_INIT_FAILED;

	return ErrorCode::SUCCESS;
}

bool SimpleNative::InitDevice()
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
		static_cast<uint32_t>(queueCreateInfos.size()),
		features,
		deviceExtensions.data(),
		static_cast<uint32_t>(deviceExtensions.size())
	))
		return false;

	dloader = device.GetLoader();

	graphicsQueue = device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
	presentQueue = device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);

	return true;
}

bool SimpleNative::InitSwapchain()
{
	Vulkan::SwapchainDetails details =
		instance->QuerySwapchainSupport(physicalDevice, &surface);
	VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details);

	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 &&
		imageCount > details.capabilities.maxImageCount)
		imageCount = details.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.Get();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchain.ChooseExtent(details.capabilities,
		pWindow->GetWidth(), pWindow->GetHeight());
	createInfo.imageArrayLayers = 1;

	// This will probably be changed later on to take in a parameter in this
	// function.
	// This parameter in the swapchain specifies what the images in the
	// swapchain are used for.
	// This may seem familiar if you have done computer graphics before,
	// as this is only the color attachment. There are others too, such as
	// the depth buffer.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);
	if (queueIndices.graphicsFamilyIndex != queueIndices.presentFamilyIndex)
	{
		if (!queueIndices.hasPresentFamily)
			return false;

		uint32_t queueFamilyIndices[] =
		{
			queueIndices.graphicsFamilyIndex,
			queueIndices.presentFamilyIndex
		};

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (!swapchain.Init(&surface, &device, details,
		&createInfo))
		return false;

	swapchainImages = swapchain.GetImages();
	if (!swapchain.GetImageViews(&swapchainImageViews))
		return false;

	return true;
}

bool SimpleNative::InitRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain.GetImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	/*VkAttachmentDescription depthAttachment{};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] =
	{
		colorAttachment,
	};

	VkRenderPassCreateInfo desc{};
	desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	desc.attachmentCount = 1;
	desc.pAttachments = attachments;
	desc.subpassCount = 1;
	desc.pSubpasses = &subpass;
	desc.dependencyCount = 1;
	desc.pDependencies = &dependency;

	if (dloader->vkCreateRenderPass(device.Get(), &desc, nullptr, &renderPass)
		!= VK_SUCCESS)
		return false;

	return true;
}

VkSurfaceFormatKHR SimpleNative::ChooseSurfaceFormat(SwapchainDetails details)
{
	for (const auto& availableFormat : details.formats)
		if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return details.formats[0];
}

void SimpleNative::OnDispose()
{
	dloader->vkDestroyRenderPass(device.Get(), renderPass, nullptr);

	swapchain.Dispose();
	device.Dispose();
	surface.Dispose();
}