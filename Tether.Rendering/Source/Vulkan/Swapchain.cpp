#include <Tether/Rendering/Vulkan/Swapchain.hpp>

#include <algorithm>
#include <cmath>

namespace Tether::Rendering::Vulkan
{
	Swapchain::Swapchain(
		GraphicsContext& context,
		const SwapchainDetails& details,
		VkSurfaceFormatKHR surfaceFormat,
		VkSurfaceKHR surface,
		uint32_t width, uint32_t height,
		bool vsync
	)
		:
		m_Instance(context.GetInstance()),
		m_Iloader(context.GetInstanceLoader()),
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader())
	{
		m_ImageCount = FindImageCount(details);
		m_ImageExtent = ChooseExtent(details.capabilities, width, height);
		m_ImageFormat = surfaceFormat.format;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.minImageCount = m_ImageCount;
		createInfo.imageFormat = m_ImageFormat;
		createInfo.imageExtent = m_ImageExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = ChoosePresentMode(details.presentModes, vsync);
		createInfo.clipped = true;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (m_Dloader.vkCreateSwapchainKHR(m_Device, &createInfo, nullptr,
			&m_Swapchain) != VK_SUCCESS)
			throw std::runtime_error("Swapchain creation failed");
	}

	Swapchain::~Swapchain()
	{
		m_Dloader.vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
	}

	VkPresentModeKHR Swapchain::ChoosePresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync)
	{
		// Fifo is used for vsync. Fifo relaxed might work too.
		// An option to use fifo relaxed may be added later.
		if (vsync)
			return VK_PRESENT_MODE_FIFO_KHR;

		// Mailbox is preferred for vsync disabled, but immediate works too.
		// If neither are supported, fifo is used.

		bool immediateSupported = false;
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;

			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				immediateSupported = true;
		}

		if (immediateSupported)
			return VK_PRESENT_MODE_IMMEDIATE_KHR;

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::ChooseExtent(
		const VkSurfaceCapabilitiesKHR& capabilities,
		uint64_t width, uint64_t height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		VkExtent2D extent =
		{
			std::clamp((uint32_t)width,
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width
			),
			std::clamp((uint32_t)height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height
			)
		};

		return extent;
	}

	uint32_t Swapchain::FindImageCount(const SwapchainDetails& details)
	{
		uint32_t imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 &&
			imageCount > details.capabilities.maxImageCount)
			imageCount = details.capabilities.maxImageCount;

		return imageCount;
	}

	std::vector<VkImage> Swapchain::GetImages()
	{
		uint32_t numImages;
		m_Dloader.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numImages,
			nullptr);

		std::vector<VkImage> swapchainImages(numImages);
		m_Dloader.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &numImages,
			swapchainImages.data());

		return swapchainImages;
	}

	std::vector<VkImageView> Swapchain::CreateImageViews()
	{
		std::vector<VkImageView> vec;
		std::vector<VkImage> images = GetImages();

		vec.resize(images.size());
		for (uint64_t i = 0; i < images.size(); i++)
		{
			VkImage image = images[i];

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_ImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (m_Dloader.vkCreateImageView(m_Device, &createInfo, nullptr,
				&vec[i]) != VK_SUCCESS)
				throw std::runtime_error("Swapchain image view creation failed");
		}

		return vec;
	}

	uint32_t Swapchain::GetImageCount()
	{
		return m_ImageCount;
	}

	VkExtent2D Swapchain::GetExtent()
	{
		return m_ImageExtent;
	}

	VkSwapchainKHR Swapchain::Get()
	{
		return m_Swapchain;
	}
}
