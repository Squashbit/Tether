#pragma once

#include <Tether/Math/Types.hpp>

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/Resources/BufferedImage.hpp>

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
			VertexBuffer* pRectBuffer
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

		BufferedImage* m_pImage = nullptr;
	};
}