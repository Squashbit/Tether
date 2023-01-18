#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/Vulkan/UniformBuffer.hpp>
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

		Image(VulkanRenderer* pRenderer);
		TETHER_NO_COPY(Image);

		void AddToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) override;
	private:
		void OnObjectUpdate() override;

		Uniforms uniforms;

		DescriptorPool pool;
		UniformBuffer uniformBuffer;

		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;
		VertexBuffer* pRectBuffer = nullptr;
		VmaAllocator allocator = nullptr;

		VulkanRenderer* pVkRenderer = nullptr;
	};
}