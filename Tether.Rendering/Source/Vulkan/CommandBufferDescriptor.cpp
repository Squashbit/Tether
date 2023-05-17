#include <Tether/Rendering/Vulkan/CommandBufferDescriptor.hpp>

namespace Tether::Rendering::Vulkan
{
	CommandBufferDescriptor::CommandBufferDescriptor(VkCommandBuffer commandBuffer,
		const DeviceLoader& dloader)
		:
		commandBuffer(commandBuffer),
		m_Dloader(dloader)
	{

	}

	void CommandBufferDescriptor::BindIfNotBound(Pipeline* pPipeline)
	{
		if (pBoundPipeline == pPipeline)
			return;

		m_Dloader.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pPipeline->Get());

		pBoundPipeline = pPipeline;
	}

	void CommandBufferDescriptor::BindIfNotBound(VertexBuffer* pVertexBuffer)
	{
		if (pBoundVertexBuffer == pVertexBuffer)
			return;

		VkBuffer vbuffers[] = { pVertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		m_Dloader.vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
		m_Dloader.vkCmdBindIndexBuffer(commandBuffer, pVertexBuffer->GetIndexBuffer(),
			0, VK_INDEX_TYPE_UINT32);

		pBoundVertexBuffer = pVertexBuffer;
	}

	VkCommandBuffer CommandBufferDescriptor::Get() const
	{
		return commandBuffer;
	}
}