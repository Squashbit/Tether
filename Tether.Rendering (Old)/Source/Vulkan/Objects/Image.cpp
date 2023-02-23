#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	Image::Image(
		Context& context,
		Pipeline& pipeline,
		VertexBuffer& rectBuffer
	)
		:
		Objects::Image(this),
		m_Device(context.device),
		m_Dloader(context.deviceLoader),
		m_Allocator(context.allocator),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer)
	{}

	void Image::SetImage(Resources::BufferedImage& image)
	{
		if (image.GetRenderer() != m_pRenderer)
			throw std::runtime_error("BufferedImage created with a different renderer");

		m_pImage = (BufferedImage*)&image;
	}

	void Image::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		if (!m_pImage)
			return;

		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(&m_Pipeline);
		commandBuffer.BindVertexBufferIfNotBound(&m_RectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x;
		pushConstants.position.y = y;
		pushConstants.scale.x = width;
		pushConstants.scale.y = height;
		
		m_Dloader.vkCmdPushConstants(
			vkCommandBuffer, m_Pipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdBindDescriptorSets(
			vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_Pipeline.GetLayout(), 0,
			1, m_pImage->GetSetAtIndex(index),
			0, nullptr
		);

		m_Dloader.vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_RectBuffer.GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
