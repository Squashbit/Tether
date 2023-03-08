#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan::TypeNames
{
	TETHER_EXPORT
	const char* GetVulkanTypeAsString(VkObjectType type);
}