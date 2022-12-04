#include <Tether/Module/Rendering/Vulkan/ObjectNatives/RectangleNative.hpp>

using namespace Tether::Rendering::Vulkan::Natives;

RectangleNative::RectangleNative(IVkContextNative* pContextNative)
	:
	VkObjectNative(pContextNative)
{

}

void RectangleNative::AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index)
{
	DeviceLoader* dloader = pContextNative->GetDeviceLoader();
	VertexBuffer* pBuffer = pContextNative->GetRectangleBuffer();

	VkBuffer vbuffers[] = { pBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	dloader->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
	dloader->vkCmdBindIndexBuffer(commandBuffer, pBuffer->GetIndexBuffer(), 0,
		VK_INDEX_TYPE_UINT32);

	dloader->vkCmdDrawIndexed(
		commandBuffer,
		static_cast<uint32_t>(pBuffer->GetVertexCount()),
		1, 0, 0, 0
	);
}