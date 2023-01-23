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
		struct Uniforms
		{
			Math::Vector2f position;
			Math::Vector2f scale;
		};

		Image(
			Renderer* pRenderer,
			Device& device,
			VmaAllocator allocator,
			Pipeline* pPipeline,
			VertexBuffer* pRectBuffer,
			VkDescriptorSetLayout pipelineSetLayout,
			uint32_t swapchainImageCount
		);
		TETHER_NO_COPY(Image);

		void SetImage(BufferedImage* image) override;

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		void OnObjectUpdate() override;

		Device& m_Device;
		DeviceLoader* m_Dloader;
		VmaAllocator m_Allocator = nullptr;
		uint32_t m_SwapchainImageCount;

		Pipeline* m_pPipeline;
		VertexBuffer* m_pRectBuffer;

		DescriptorPool m_Pool;
		DescriptorSet m_Set;
		UniformBuffer m_UniformBuffer;

		Uniforms m_Uniforms;
	};
}