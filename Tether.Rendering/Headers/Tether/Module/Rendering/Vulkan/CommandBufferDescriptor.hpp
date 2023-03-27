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
		CommandBufferDescriptor(VkCommandBuffer commandBuffer, 
			const DeviceLoader& dloader);
		TETHER_NO_COPY(CommandBufferDescriptor);

		void BindIfNotBound(Pipeline* pPipeline);
		void BindIfNotBound(VertexBuffer* pVertexBuffer);

		VkCommandBuffer Get() const;
	private:
		const DeviceLoader& m_Dloader;

		Pipeline* pBoundPipeline = nullptr;
		VertexBuffer* pBoundVertexBuffer = nullptr;

		VkCommandBuffer commandBuffer = nullptr;
	};
}