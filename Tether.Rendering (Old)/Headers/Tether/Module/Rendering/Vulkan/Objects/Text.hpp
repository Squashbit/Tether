#pragma once

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/Resources/Font.hpp>

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Text : public Objects::Text, public ObjectRenderer
	{
	public:
		struct PushConstants
		{
			Math::Vector2f position;
			Math::Vector2f scale;
			Math::Vector4f color;
			Math::Vector2<int> windowSize;
		};

		Text(VulkanContext& context, VkExtent2D& swapchainExtent, 
			Pipeline& pipeline, VertexBuffer& rectBuffer);
		TETHER_NO_COPY(Text);

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		void RenderCharacter(CommandBufferDescriptor& commandBuffer,
			uint32_t index, Font::Character& character, float offset);

		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;
		VkExtent2D& m_SwapchainExtent;
		Pipeline& m_Pipeline;
		VertexBuffer& m_RectBuffer;

		PushConstants pushConstants;
	};
}