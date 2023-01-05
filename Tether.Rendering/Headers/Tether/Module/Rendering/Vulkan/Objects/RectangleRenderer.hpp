#pragma once

#include <Tether/Module/Rendering/Vulkan/Common/Uniforms.hpp>
#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT RectangleRenderer : public ObjectRenderer
	{
	public:
		RectangleRenderer(VulkanUIRenderer* pRenderer);
		~RectangleRenderer();
		TETHER_NO_COPY(RectangleRenderer);

		void OnRenderFrame(uint32_t currentFrame) override;
		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) override;
	private:
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		Transform transform;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VmaAllocation> uniformAllocations;
		std::vector<VmaAllocationInfo> uniformAllocInfos;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;
		VertexBuffer* pRectBuffer = nullptr;
		VmaAllocator allocator = nullptr;
	};
}