#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	Image::Image(
		GraphicsContext& context,
		Pipeline& pipeline,
		VertexBuffer& rectBuffer,
		VkExtent2D& swapchainExtent
	)
		:
		Objects::Image((ObjectRenderer&)*this),
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader()),
		m_Allocator(context.GetAllocator()),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer),
		m_SwapchainExtent(swapchainExtent)
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

		commandBuffer.BindIfNotBound(&m_Pipeline);
		commandBuffer.BindIfNotBound(&m_RectBuffer);

		PushConstants pushConstants;
		pushConstants.position.x = x / m_SwapchainExtent.width;
		pushConstants.position.y = y / m_SwapchainExtent.height;
		pushConstants.scale.x = width / m_SwapchainExtent.width;
		pushConstants.scale.y = height / m_SwapchainExtent.height;
		
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
