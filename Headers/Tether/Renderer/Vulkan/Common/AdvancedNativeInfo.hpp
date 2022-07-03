#ifndef _TETHER_RENDERER_VULKAN_ADVANCEDNATIVEINFO_HPP
#define _TETHER_RENDERER_VULKAN_ADVANCEDNATIVEINFO_HPP

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	struct AdvancedNativeInfo
	{
		VkInstance instance;
		VkDevice device;
		VkSwapchainKHR swapchain;
	};
}

#endif //_TETHER_RENDERER_VULKAN_ADVANCEDNATIVEINFO_HPP