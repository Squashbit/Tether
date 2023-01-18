#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <iostream>

namespace Tether::Rendering::Vulkan
{
	Image::Image(VulkanRenderer* pVkRenderer)
		:
		Objects::Image(pVkRenderer),
		pool(pVkRenderer->GetDevice(), pVkRenderer->GetSwapchainImageCount()),
		uniformBuffer(
			pVkRenderer->GetAllocator(), pVkRenderer->GetDevice(),
			&pool, pVkRenderer->GetDescriptorSetLayout(),
			sizeof(Uniforms), pVkRenderer->GetSwapchainImageCount()
		)
	{
		this->pObjectRenderer = this;
		this->pVkRenderer = pVkRenderer;

		this->device = pVkRenderer->GetDevice();
		this->dloader = device->GetLoader();
		this->pRectBuffer = pVkRenderer->GetRectangleBuffer();
		this->allocator = pVkRenderer->GetAllocator();
	}

	void Image::OnObjectUpdate()
	{
		uniforms.position.x = x;
		uniforms.position.y = y;
		uniforms.scale.x = width;
		uniforms.scale.y = height;

		for (uint32_t i = 0; i < pVkRenderer->GetSwapchainImageCount(); i++)
			memcpy(uniformBuffer.GetMappedData(i), &uniforms, sizeof(Uniforms));
	}

	void Image::AddToCommandBuffer(VkCommandBuffer commandBuffer,
		uint32_t index)
	{
		VkBuffer vbuffers[] = { pRectBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		dloader->vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
		dloader->vkCmdBindIndexBuffer(commandBuffer, pRectBuffer->GetIndexBuffer(), 0,
			VK_INDEX_TYPE_UINT32);

		dloader->vkCmdBindDescriptorSets(
			commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pVkRenderer->GetPipeline()->GetLayout(), 0,
			1, uniformBuffer.GetSetAtIndex(index),
			0, nullptr
		);

		dloader->vkCmdDrawIndexed(
			commandBuffer,
			static_cast<uint32_t>(pRectBuffer->GetVertexCount()),
			1, 0, 0, 0
		);
	}
}
