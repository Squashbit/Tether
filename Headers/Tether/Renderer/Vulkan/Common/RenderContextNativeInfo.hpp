#ifndef _TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVEINFO_HPP
#define _TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVEINFO_HPP

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	struct RenderContextNativeInfo
	{
		VkInstance instance;
		VkDevice device;
		VkSwapchainKHR swapchain;
	};
}

#endif //_TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVEINFO_HPP