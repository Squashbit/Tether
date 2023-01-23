#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanBufferedImage.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <iostream>

namespace Tether::Rendering::Vulkan
{
	Image::Image(
		Renderer* pRenderer,
		Device& device,
		VmaAllocator allocator,
		Pipeline* pPipeline,
		VertexBuffer* pRectBuffer,
		VkDescriptorSetLayout pipelineSetLayout,
		uint32_t swapchainImageCount
	)
		:
		Objects::Image(pRenderer),
		m_Device(device),
		m_Dloader(m_Device.GetLoader()),
		m_Allocator(allocator),
		m_pPipeline(pPipeline),
		m_pRectBuffer(pRectBuffer),
		m_SwapchainImageCount(swapchainImageCount),
		m_Pool(&m_Device, swapchainImageCount),
		m_Set(&m_Device, m_Pool, pipelineSetLayout, m_SwapchainImageCount),
		m_UniformBuffer(allocator, &device, sizeof(Uniforms), m_Set, 0)
	{
		this->pObjectRenderer = this;
	}

	void Image::SetImage(BufferedImage* image)
	{
		VulkanBufferedImage* vkImage = (VulkanBufferedImage*)image;
		m_Set.UpdateSets(vkImage, 1);
	}

	void Image::OnObjectUpdate()
	{
		m_Uniforms.position.x = x;
		m_Uniforms.position.y = y;
		m_Uniforms.scale.x = width;
		m_Uniforms.scale.y = height;

		for (uint32_t i = 0; i < m_SwapchainImageCount; i++)
			memcpy(m_UniformBuffer.GetMappedData(i), &m_Uniforms, sizeof(Uniforms));
	}

	void Image::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(m_pPipeline);
		commandBuffer.BindVertexBufferIfNotBound(m_pRectBuffer);

		m_Dloader->vkCmdBindDescriptorSets(
			vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pPipeline->GetLayout(), 0,
			1, &m_Set.GetSetAtIndex(index),
			0, nullptr
		);

		m_Dloader->vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_pRectBuffer->GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
