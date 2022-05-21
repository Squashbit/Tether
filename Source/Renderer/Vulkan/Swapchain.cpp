#include <Tether/Renderer/Vulkan/Swapchain.hpp>

#include <algorithm>
#include <cmath>

using namespace Tether::Vulkan;

bool Swapchain::Init(
	Surface* pSurface,
	Device* pDevice,
	SwapchainDetails details,
	VkSwapchainCreateInfoKHR* createInfo
)
{
	if (initialized)
		return false;
	
	this->pDevice = pDevice;
	this->imageFormat = createInfo->imageFormat;
	this->extent = createInfo->imageExtent;
	this->imageCount = createInfo->minImageCount;
	
	if (vkCreateSwapchainKHR(pDevice->Get(), 
		createInfo, nullptr, &swapchain))
		return false;
	
	initialized = true;
	return true;
}

uint32_t Swapchain::GetImageCount()
{
	return imageCount;
}

std::vector<VkImage> Swapchain::GetImages()
{
	uint32_t numImages;
	vkGetSwapchainImagesKHR(pDevice->Get(), swapchain, &numImages,
		nullptr);
	
	std::vector<VkImage> swapchainImages(numImages);
	vkGetSwapchainImagesKHR(pDevice->Get(), swapchain, &numImages,
		swapchainImages.data());

	return swapchainImages;
}

bool Swapchain::GetImageViews(std::vector<VkImageView>* pVec)
{
	std::vector<VkImage> images = GetImages();

	pVec->resize(images.size());
	for (uint64_t i = 0; i < images.size(); i++)
	{
		VkImage image = images[i];

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		
		if (vkCreateImageView(pDevice->Get(), &createInfo, nullptr,
			&pVec->at(i)) != VK_SUCCESS)
			return false;
	}

	return true;
}

VkExtent2D Swapchain::GetExtent()
{
	return extent;
}

VkFormat Swapchain::GetImageFormat()
{
	return imageFormat;
}

VkSwapchainKHR Swapchain::Get()
{
	return swapchain;
}

void Swapchain::OnDispose()
{
	vkDestroySwapchainKHR(pDevice->Get(), swapchain, nullptr);
}

VkPresentModeKHR Swapchain::ChoosePresentMode(
	std::vector<VkPresentModeKHR>& availablePresentModes, bool vsync) 
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
	VkSurfaceCapabilitiesKHR& capabilities,
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
