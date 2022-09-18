#ifndef _TETHER_VULKAN_QUEUEFAMILYINDICES_HPP
#define _TETHER_VULKAN_QUEUEFAMILYINDICES_HPP

#include <cstdint>

namespace Tether::Rendering::Vulkan
{
    struct QueueFamilyIndices
	{
		bool hasGraphicsFamily = false;
		bool hasPresentFamily = false;
		uint32_t graphicsFamilyIndex = 0;
		uint32_t presentFamilyIndex = 0;
	};
}

#endif //_TETHER_VULKAN_QUEUEFAMILYINDICES_HPP