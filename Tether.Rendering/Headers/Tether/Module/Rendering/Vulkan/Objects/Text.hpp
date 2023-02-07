#pragma once

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/Resources/Font.hpp>

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Text : public Objects::Text, public ObjectRenderer
	{
	public:
		struct PushConstants
		{
			Math::Vector2f position;
			Math::Vector2f scale;
			Math::Vector4f color;
			Math::Vector2<int> windowSize;
		};

		Text(Device& device, Pipeline& pipeline, VertexBuffer& rectBuffer,
			SimpleWindow& window);
		TETHER_NO_COPY(Text);

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:
		void RenderCharacter(CommandBufferDescriptor& commandBuffer,
			uint32_t index, Font::Character& character, float offset);

		Device& m_Device;
		DeviceLoader* m_Dloader;

		Pipeline& m_Pipeline;
		VertexBuffer& m_RectBuffer;
		SimpleWindow& m_Window;

		PushConstants pushConstants;
	};
}