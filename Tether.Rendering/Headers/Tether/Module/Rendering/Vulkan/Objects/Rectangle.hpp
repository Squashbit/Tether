#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Rectangle : public Objects::Rectangle, public ObjectRenderer
	{
	public:
		struct Uniforms
		{
			Math::Vector2f position;
			Math::Vector2f scale;
			Math::Vector3f color;
		};

		Rectangle(VulkanUIRenderer* pRenderer);
		~Rectangle();
		TETHER_NO_COPY(Rectangle);

		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) override;
	private:
		void OnObjectUpdate() override;

		void CreateDescriptorPool();
		void CreateDescriptorSets();

		Uniforms uniforms;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VmaAllocation> uniformAllocations;
		std::vector<VmaAllocationInfo> uniformAllocInfos;

		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;

		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;
		VertexBuffer* pRectBuffer = nullptr;
		VmaAllocator allocator = nullptr;

		VulkanUIRenderer* pVkRenderer = nullptr;
	};
}