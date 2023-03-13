#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/GraphicsContext.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <optional>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(VulkanContext& context);
		~VulkanGraphicsContext();

		Scope<Objects::Rectangle> CreateRectangleObject() override;
		Scope<Objects::Image> CreateImageObject() override;
		Scope<Objects::Text> CreateTextObject() override;

		Scope<Resources::BufferedImage> CreateBufferedImage(
			const Resources::BufferedImageInfo& info) override;
		Scope<Resources::Font> CreateFont(const std::string& fontPath) override;
	private:
		void CreateVertexBuffers();
		void CreateSampler();

		VulkanContext& m_Context;

		std::optional<VertexBuffer> square;
		VkSampler sampler;
	};
}