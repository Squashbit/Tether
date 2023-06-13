#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Renderer.hpp>
#include <Tether/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/Resources/Font.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(GraphicsContext& graphicsContext);

		void FillRect(float x, float y, float width, float height,
			Math::Vector4f color) override;
		void DrawImage(float x, float y, float width, float height,
			Resources::BufferedImage& image) override;

		void DrawText(
			float x, float y,
			std::string_view text, Resources::Font& font,
			Math::Vector4f color = Math::Vector4f(1.0f),
			float scale = 1.0f, TextJustify justify = TextJustify::RIGHT
		) override;
	protected:
		void StartNewFrame(uint32_t commandBufferIndex, 
			VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent);
		void SetPipelines(Pipeline& imagePipeline);
	private:
		void RenderCharacter(
			Math::Vector4f color,
			Font::Character& character, 
			float scale, float x, float y, 
			float xOffset, float yOffset
		);

		GraphicsContext& m_GraphicsContext;

		VertexBuffer& m_Square;
		const DeviceLoader& m_Dloader;

		Pipeline* m_pRectPipeline = nullptr;
		Pipeline* m_pImagePipeline = nullptr;
		Pipeline* m_pTextPipeline = nullptr;

		uint32_t m_CBufIndex = 0;
		VkCommandBuffer m_CommandBuffer = nullptr;
		VkExtent2D m_SwapchainExtent = {};
	};
}