#pragma once

#include <Tether/Module/Rendering/Vulkan/ObjectNatives/VkObjectNative.hpp>

namespace Tether::Rendering::Vulkan::Natives
{
	class TETHER_EXPORT RectangleNative : public VkObjectNative
	{
	public:
		RectangleNative(VulkanUIRenderer* pRenderer);
		TETHER_NO_COPY(RectangleNative);

		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
	private:
		DeviceLoader* dloader;
		VertexBuffer* pRectBuffer;
	};
}