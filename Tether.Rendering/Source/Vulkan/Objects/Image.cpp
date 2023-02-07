#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering::Vulkan
{
	Image::Image(
		Device& device,
		VmaAllocator allocator,
		Pipeline* pPipeline,
		VertexBuffer* pRectBuffer
	)
		:
		Objects::Image(this),
		m_Device(device),
		m_Dloader(m_Device.GetLoader()),
		m_Allocator(allocator),
		m_pPipeline(pPipeline),
		m_pRectBuffer(pRectBuffer)
	{}

	void Image::SetImage(Resources::BufferedImage* image)
	{
		if (!image)
		{
			m_pImage = nullptr;
			return;
		}

		if (image->GetRenderer() != m_pRenderer)
			throw RendererException("BufferedImage created with a different renderer");

		m_pImage = (BufferedImage*)image;
	}

	void Image::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		if (!m_pImage)
			return;

		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(m_pPipeline);
		commandBuffer.BindVertexBufferIfNotBound(m_pRectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x;
		pushConstants.position.y = y;
		pushConstants.scale.x = width;
		pushConstants.scale.y = height;
		
		m_Dloader->vkCmdPushConstants(
			vkCommandBuffer, m_pPipeline->GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader->vkCmdBindDescriptorSets(
			vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pPipeline->GetLayout(), 0,
			1, m_pImage->GetSetAtIndex(index),
			0, nullptr
		);

		m_Dloader->vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_pRectBuffer->GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
