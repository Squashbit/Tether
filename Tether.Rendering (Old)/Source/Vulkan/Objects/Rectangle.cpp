#include <Tether/Module/Rendering/Vulkan/Objects/Rectangle.hpp>

namespace Tether::Rendering::Vulkan
{
	Rectangle::Rectangle(
		Context& context,
		Pipeline& pipeline,
		VertexBuffer& rectBuffer
	)
		:
		Objects::Rectangle(this),
		m_Device(context.device),
		m_Allocator(context.allocator),
		m_Dloader(context.deviceLoader),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer)
	{}

	void Rectangle::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(&m_Pipeline);
		commandBuffer.BindVertexBufferIfNotBound(&m_RectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x;
		pushConstants.position.y = y;
		pushConstants.scale.x = width;
		pushConstants.scale.y = height;
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
