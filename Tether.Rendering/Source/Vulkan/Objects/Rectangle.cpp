#include <Tether/Module/Rendering/Vulkan/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <iostream>

namespace Tether::Rendering::Vulkan
{
	Rectangle::Rectangle(
		Device& device,
		VmaAllocator allocator,
		Pipeline* pPipeline,
		VertexBuffer* pRectBuffer,
		uint32_t framesInFlight
	)
		:
		Objects::Rectangle(this),
		m_Device(device),
		m_Dloader(m_Device.GetLoader()),
		m_Allocator(allocator),
		m_pPipeline(pPipeline),
		m_pRectBuffer(pRectBuffer)
	{}

	void Rectangle::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(m_pPipeline);
		commandBuffer.BindVertexBufferIfNotBound(m_pRectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x;
		pushConstants.position.y = y;
		pushConstants.scale.x = width;
		pushConstants.scale.y = height;
		pushConstants.color.x = color.GetR();
		pushConstants.color.y = color.GetG();
		pushConstants.color.z = color.GetB();

		m_Dloader->vkCmdPushConstants(
			vkCommandBuffer, m_pPipeline->GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader->vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_pRectBuffer->GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
