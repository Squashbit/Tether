#pragma once

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	struct AdvancedNativeInfo
	{
		VkInstance instance;
		VkDevice device;
		VkSwapchainKHR swapchain;
	};
}
