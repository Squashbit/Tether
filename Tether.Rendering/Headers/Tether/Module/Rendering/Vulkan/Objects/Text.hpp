#pragma once

#include <Tether/Module/Rendering/Vulkan/Objects/ObjectRenderer.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Text : public Objects::Text, public ObjectRenderer
	{
	public:
		Text();
		TETHER_NO_COPY(Text);

		void SetText(const std::string& text) override;

		void AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
			uint32_t index) override;
	private:

	};
}