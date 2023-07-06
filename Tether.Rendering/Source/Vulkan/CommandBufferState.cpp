#include <Tether/Rendering/Vulkan/CommandBufferState.hpp>

namespace Tether::Rendering::Vulkan
{
	CommandBufferState::CommandBufferState(VkDevice device, const DeviceLoader& dloader,
		VkCommandBuffer commandBuffer)
		:
		m_Device(device),
		m_Dloader(dloader),
		m_CommandBuffer(commandBuffer)
	{
		TETHER_ASSERT(device);
	}

	void CommandBufferState::BindPipelineIfNotBound(VkPipeline pipeline, 
		VkPipelineBindPoint bindPoint)
	{
		if (!m_CommandBuffer || pipeline == m_pBoundPipeline)
			return;

		m_Dloader.vkCmdBindPipeline(m_CommandBuffer, bindPoint, pipeline);
		m_pBoundPipeline = pipeline;
	}

	void CommandBufferState::BindVertexBufferIfNotBound(VkBuffer vertexBuffer)
	{
		if (!m_CommandBuffer || vertexBuffer == m_pBoundVertexBuffer)
			return;

		VkDeviceSize offsets[] = { 0 };
		m_Dloader.vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &vertexBuffer, 
			offsets);

		m_pBoundVertexBuffer = vertexBuffer;
	}

	void CommandBufferState::BindIndexBufferIfNotBound(VkBuffer indexBuffer, 
		VkIndexType indexType)
	{
		if (!m_CommandBuffer || indexBuffer == m_pBoundIndexBuffer)
			return;

		m_Dloader.vkCmdBindIndexBuffer(m_CommandBuffer, indexBuffer, 0, indexType);
		m_pBoundIndexBuffer = indexBuffer;
	}
}