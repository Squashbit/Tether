#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Rectangle : public Objects::Rectangle, public ObjectRenderer
	{
	public:
		struct PushConstants
		{
			Math::Vector2f position;
			Math::Vector2f scale;
			Math::Vector3f color;
		};

		Rectangle(
			Device& device,
			VmaAllocator allocator,
			Pipeline* pPipeline,
			VertexBuffer* pRectBuffer,
			uint32_t framesInFlight
		);
		TETHER_NO_COPY(Rectangle);

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		Device& m_Device;
		DeviceLoader* m_Dloader;
		VmaAllocator m_Allocator = nullptr;
		
		Pipeline* m_pPipeline;
		VertexBuffer* m_pRectBuffer;
	};
}