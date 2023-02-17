#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT CommandBufferDescriptor
	{
	public:
		CommandBufferDescriptor(VkCommandBuffer commandBuffer, DeviceLoader& dloader);
		TETHER_NO_COPY(CommandBufferDescriptor);

		void BindPipelineIfNotBound(Pipeline* pPipeline);
		void BindVertexBufferIfNotBound(VertexBuffer* pVertexBuffer);

		VkCommandBuffer Get();
	private:
		DeviceLoader& m_Dloader;

		Pipeline* pBoundPipeline = nullptr;
		VertexBuffer* pBoundVertexBuffer = nullptr;

		VkCommandBuffer commandBuffer = nullptr;
	};
}