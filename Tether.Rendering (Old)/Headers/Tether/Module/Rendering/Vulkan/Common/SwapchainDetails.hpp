#pragma once

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
