#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan::Natives
{
	class TETHER_EXPORT VkObjectNative : public Objects::ObjectNative
	{
	public:
		VkObjectNative() = default;
		TETHER_NO_COPY(VkObjectNative);

		virtual void AddToCommandBuffer(VkCommandBuffer commandBuffer,
			uint32_t index) {}
	};
}