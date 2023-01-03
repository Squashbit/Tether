#pragma once

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT RectangleRenderer : public ObjectRenderer
	{
	public:
		RectangleRenderer(VulkanUIRenderer* pRenderer);
		TETHER_NO_COPY(RectangleRenderer);

		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
	private:
		DeviceLoader* dloader = nullptr;
		VertexBuffer* pRectBuffer = nullptr;
	};
}