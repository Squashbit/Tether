#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT CommandBufferState
	{
	public:
		CommandBufferState(VkDevice device, const DeviceLoader& dloader, 
			VkCommandBuffer commandBuffer);

		void BindPipelineIfNotBound(VkPipeline pipeline, VkPipelineBindPoint bindPoint);
		void BindVertexBufferIfNotBound(VkBuffer vertexBuffer);
		void BindIndexBufferIfNotBound(VkBuffer indexBuffer, VkIndexType indexType);
	private:
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
		VkCommandBuffer m_CommandBuffer = nullptr;

		VkPipeline m_pBoundPipeline = nullptr;
		VkBuffer m_pBoundVertexBuffer = nullptr;
		VkBuffer m_pBoundIndexBuffer = nullptr;
	};
}