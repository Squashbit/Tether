#pragma once

#include <Tether/Module/Rendering/Vulkan/ObjectNatives/VkObjectNative.hpp>

namespace Tether::Rendering::Vulkan::Natives
{
	class TETHER_EXPORT RectangleNative : public VkObjectNative
	{
	public:
		RectangleNative() = default;
		TETHER_NO_COPY(RectangleNative);

		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
	};
}