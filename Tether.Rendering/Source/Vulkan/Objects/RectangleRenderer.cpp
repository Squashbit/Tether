#include <Tether/Module/Rendering/Vulkan/Objects/RectangleRenderer.hpp>

using namespace Tether::Rendering::Vulkan;

RectangleRenderer::RectangleRenderer(VulkanUIRenderer* pVkRenderer)
	:
	ObjectRenderer(pVkRenderer)
{
	this->dloader = pVkRenderer->GetDeviceLoader();
	this->pRectBuffer = pVkRenderer->GetRectangleBuffer();
}

void RectangleRenderer::AddToCommandBuffer(VkCommandBuffer commandBuffer,
	uint32_t index)
{
	VkBuffer vbuffers[] = { pRectBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	dloader->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
	dloader->vkCmdBindIndexBuffer(commandBuffer, pRectBuffer->GetIndexBuffer(), 0,
		VK_INDEX_TYPE_UINT32);

	dloader->vkCmdDrawIndexed(
		commandBuffer,
		static_cast<uint32_t>(pRectBuffer->GetVertexCount()),
		1, 0, 0, 0
	);
}