#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Rectangle : public Objects::Rectangle, public ObjectRenderer
	{
	public:
		struct PushConstants
		{
			Math::Vector2f position;
			Math::Vector2f scale;
			Math::Vector4f color;
		};

		Rectangle(
			VulkanContext& context,
			Pipeline& pipeline,
			VertexBuffer& rectBuffer,
			VkExtent2D& swapchainExtent
		);
		TETHER_NO_COPY(Rectangle);

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		VkDevice m_Device = nullptr;
		VmaAllocator m_Allocator = nullptr;
		DeviceLoader& m_Dloader;
		Pipeline& m_Pipeline;
		VertexBuffer& m_RectBuffer;

		VkExtent2D& m_SwapchainExtent;
	};
}