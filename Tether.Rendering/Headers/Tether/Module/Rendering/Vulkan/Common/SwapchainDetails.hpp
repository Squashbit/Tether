#ifndef _TETHER_VULKAN_SWAPCHAINDETAILS_HPP
#define _TETHER_VULKAN_SWAPCHAINDETAILS_HPP

#include <vulkan/vulkan.h>
#include <vector>

namespace Tether::Rendering::Vulkan
{
    struct SwapchainDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
}

#endif //_TETHER_VULKAN_SWAPCHAINDETAILS_HPP