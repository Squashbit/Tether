#include <Tether/Module/Rendering/Vulkan/Objects/Rectangle.hpp>

namespace Tether::Rendering::Vulkan
{
	Rectangle::Rectangle(
		VulkanContext& context,
		Pipeline& pipeline,
		VertexBuffer& rectBuffer,
		VkExtent2D& swapchainExtent
	)
		:
		Objects::Rectangle((ObjectRenderer&)*this),
		m_Device(context.device),
		m_Allocator(context.allocator),
		m_Dloader(context.deviceLoader),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer),
		m_SwapchainExtent(swapchainExtent)
	{}

	void Rectangle::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindIfNotBound(&m_Pipeline);
		commandBuffer.BindIfNotBound(&m_RectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x / m_SwapchainExtent.width;
		pushConstants.position.y = y / m_SwapchainExtent.height;
		pushConstants.scale.x = width / m_SwapchainExtent.width;
		pushConstants.scale.y = height / m_SwapchainExtent.height;
		pushConstants.color = color;
		
		m_Dloader.vkCmdPushConstants(
			vkCommandBuffer, m_Pipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_RectBuffer.GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
