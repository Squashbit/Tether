#include <Tether/Module/Rendering/Vulkan/SimpleVulkanContext.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	SimpleVulkanContext::SimpleVulkanContext(SimpleWindow& window)
		:
		m_Instance(GlobalVulkan::Get().GetInstance()),
		surface(m_Instance.Get(), m_Instance.GetLoader(), window),
		m_Device(m_Instance, surface.Get())
	{
		GetInstanceProcAddr = GlobalVulkan::Get().GetGetInstanceProcAddr();

		instance = m_Instance.Get();
		instanceLoader = m_Instance.GetLoader();
		device = m_Device.Get();
		deviceLoader = m_Device.GetLoader();
		physicalDevice = m_Device.GetPhysicalDevice();
		queueIndices = m_Instance.FindQueueFamilies(physicalDevice, surface.Get());
		graphicsQueue = m_Device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
		presentQueue = m_Device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);
		
		QuerySwapchainSupport();
		ChooseSurfaceFormat();

		CreateRenderPass();
		CreateCommandPool();
	}

	SimpleVulkanContext::~SimpleVulkanContext()
	{
		deviceLoader.vkDestroyCommandPool(device, commandPool, nullptr);
		deviceLoader.vkDestroyRenderPass(device, renderPass, nullptr);
	}

	void SimpleVulkanContext::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = surfaceFormat.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

		if (deviceLoader.vkCreateRenderPass(device, &desc, nullptr,
			&renderPass) != VK_SUCCESS)
			throw std::runtime_error("Render pass creation failed");
	}

	void SimpleVulkanContext::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = queueIndices.graphicsFamilyIndex;

		if (deviceLoader.vkCreateCommandPool(device, &createInfo, nullptr,
			&commandPool) != VK_SUCCESS)
			throw std::runtime_error("Command pool creation failed");
	}

	void SimpleVulkanContext::ChooseSurfaceFormat()
	{
		if (swapchainDetails.formats.size() == 0)
			throw std::runtime_error("No available swapchain image formats");

		for (VkSurfaceFormatKHR availableFormat : swapchainDetails.formats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				surfaceFormat = availableFormat;
				return;
			}

		surfaceFormat = swapchainDetails.formats[0];
	}

	void SimpleVulkanContext::QuerySwapchainSupport()
	{
		VkSurfaceKHR vkSurface = surface.Get();

		instanceLoader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physicalDevice,
			vkSurface, &swapchainDetails.capabilities
		);

		uint32_t formatCount;
		instanceLoader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice, vkSurface,
			&formatCount, nullptr
		);

		if (formatCount != 0)
		{
			swapchainDetails.formats.resize(formatCount);
			instanceLoader.vkGetPhysicalDeviceSurfaceFormatsKHR(
				physicalDevice,
				vkSurface, &formatCount,
				swapchainDetails.formats.data()
			);
		}

		uint32_t presentModeCount;
		instanceLoader.vkGetPhysicalDeviceSurfacePresentModesKHR(
			physicalDevice,
			vkSurface, &presentModeCount,
			swapchainDetails.presentModes.data()
		);

		if (presentModeCount != 0)
		{
			swapchainDetails.presentModes.resize(presentModeCount);
			instanceLoader.vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice,
				vkSurface, &presentModeCount,
				swapchainDetails.presentModes.data()
			);
		}
	}
}