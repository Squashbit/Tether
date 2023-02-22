#pragma once

#include <cstdint>

namespace Tether::Rendering::Vulkan
{
    struct QueueFamilyIndices
	{
		bool hasGraphicsFamily = false;
		uint32_t graphicsFamilyIndex = 0;
	};
}
