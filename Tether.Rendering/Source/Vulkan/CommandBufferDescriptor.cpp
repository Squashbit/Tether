#include <Tether/Module/Rendering/Vulkan/CommandBufferDescriptor.hpp>

namespace Tether::Rendering::Vulkan
{
	CommandBufferDescriptor::CommandBufferDescriptor(VkCommandBuffer commandBuffer,
		DeviceLoader* dloader)
		:
		commandBuffer(commandBuffer),
		dloader(dloader)
	{

	}

	void CommandBufferDescriptor::BindPipelineIfNotBound(Pipeline* pPipeline)
	{
		if (pBoundPipeline == pPipeline)
			return;

		dloader->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pPipeline->Get());

		pBoundPipeline = pPipeline;
	}

	void CommandBufferDescriptor::BindVertexBufferIfNotBound(VertexBuffer* pVertexBuffer)
	{
		if (pBoundVertexBuffer == pVertexBuffer)
			return;

		VkBuffer vbuffers[] = { pVertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		dloader->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
		dloader->vkCmdBindIndexBuffer(commandBuffer, pVertexBuffer->GetIndexBuffer(),
			0, VK_INDEX_TYPE_UINT32);

		pBoundVertexBuffer = pVertexBuffer;
	}

	VkCommandBuffer CommandBufferDescriptor::Get()
	{
		return commandBuffer;
	}
}