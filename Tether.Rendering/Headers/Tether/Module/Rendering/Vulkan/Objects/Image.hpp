#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>

#include <Tether/Module/Rendering/Objects/Image.hpp>

#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Image : public Objects::Image, public ObjectRenderer
	{
	public:
		struct PushConstants
		{
			Math::Vector2f position;
			Math::Vector2f scale;
		};

		Image(
			Device& device,
			VmaAllocator allocator,
			Pipeline* pPipeline,
			VertexBuffer* pRectBuffer,
			VkDescriptorSetLayout pipelineSetLayout,
			uint32_t framesInFlight
		);
		TETHER_NO_COPY(Image);

		void SetImage(Resources::BufferedImage* image) override;

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		Device& m_Device;
		DeviceLoader* m_Dloader;
		VmaAllocator m_Allocator = nullptr;
		
		Pipeline* m_pPipeline;
		VertexBuffer* m_pRectBuffer;

		std::optional<DescriptorPool> m_Pool;
		std::optional<DescriptorSet> m_Set;
	};
}