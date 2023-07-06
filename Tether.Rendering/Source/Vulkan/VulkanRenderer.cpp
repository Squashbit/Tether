#include <Tether/Rendering/Vulkan/VulkanRenderer.hpp>

#include <Tether/Rendering/Vulkan/Resources/BufferedImage.hpp>

#include <stdexcept>

#define TETHER_THROW_IF_NO_COMMAND_BUFFER() \
	if (!m_CommandBuffer) \
		throw std::logic_error("The function StartRender " \
			"must be called before rendering. " \
			"The StartRender function must call StartNewFrame.")

namespace Tether::Rendering::Vulkan
{
	VulkanRenderer::VulkanRenderer(GraphicsContext& graphicsContext,
		Pipeline& rectPipeline, Pipeline& imagePipeline, 
		Pipeline& textPipeline)
		:
		m_GraphicsContext(graphicsContext),
		m_Square(m_GraphicsContext.GetSquareBuffer()),
		m_Dloader(m_GraphicsContext.GetDeviceLoader()),
		m_RectPipeline(rectPipeline),
		m_ImagePipeline(imagePipeline),
		m_TextPipeline(textPipeline)
	{}

	void VulkanRenderer::FillRect(float x, float y, float width, float height, 
		Math::Vector4f color)
	{
		TETHER_THROW_IF_NO_COMMAND_BUFFER();

		m_CommandBufferState->BindPipelineIfNotBound(m_RectPipeline.Get(),
			VK_PIPELINE_BIND_POINT_GRAPHICS);
		m_CommandBufferState->BindVertexBufferIfNotBound(m_Square.GetBuffer());
		m_CommandBufferState->BindIndexBufferIfNotBound(m_Square.GetIndexBuffer(),
			VK_INDEX_TYPE_UINT32);

		RectanglePushConstants pushConstants;
		pushConstants.position.x = x / m_SwapchainExtent.width;
		pushConstants.position.y = y / m_SwapchainExtent.height;
		pushConstants.scale.x = width / m_SwapchainExtent.width;
		pushConstants.scale.y = height / m_SwapchainExtent.height;
		pushConstants.color = color;

		m_Dloader.vkCmdPushConstants(
			m_CommandBuffer, m_RectPipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(RectanglePushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdDrawIndexed(
			m_CommandBuffer,
			static_cast<uint32_t>(m_Square.GetVertexCount()),
			1, 0, 0, 0
		);
	}

	void VulkanRenderer::DrawImage(float x, float y, float width, float height, 
		Resources::BufferedImage& image)
	{
		TETHER_THROW_IF_NO_COMMAND_BUFFER();

		BufferedImage& vkImage = (BufferedImage&)image;

		ImagePushConstants pushConstants;
		pushConstants.position.x = x / m_SwapchainExtent.width;
		pushConstants.position.y = y / m_SwapchainExtent.height;
		pushConstants.scale.x = width / m_SwapchainExtent.width;
		pushConstants.scale.y = height / m_SwapchainExtent.height;

		m_CommandBufferState->BindPipelineIfNotBound(m_ImagePipeline.Get(),
			VK_PIPELINE_BIND_POINT_GRAPHICS);
		m_CommandBufferState->BindVertexBufferIfNotBound(m_Square.GetBuffer());
		m_CommandBufferState->BindIndexBufferIfNotBound(m_Square.GetIndexBuffer(),
			VK_INDEX_TYPE_UINT32);

		m_Dloader.vkCmdPushConstants(
			m_CommandBuffer, m_ImagePipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(ImagePushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdBindDescriptorSets(
			m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_ImagePipeline.GetLayout(), 0,
			1, vkImage.GetSetAtIndex(m_CBufIndex),
			0, nullptr
		);

		m_Dloader.vkCmdDrawIndexed(
			m_CommandBuffer,
			static_cast<uint32_t>(m_Square.GetVertexCount()),
			1, 0, 0, 0
		);
	}

	void VulkanRenderer::DrawText(float x, float y, std::string_view text, 
		Resources::Font& font, Math::Vector4f color, float scale, TextJustify justify)
	{
		TETHER_THROW_IF_NO_COMMAND_BUFFER();
		
		Font& vkFont = (Font&)font;
		vkFont.LoadCharactersFromString(text);

		m_CommandBufferState->BindPipelineIfNotBound(m_TextPipeline.Get(),
			VK_PIPELINE_BIND_POINT_GRAPHICS);
		m_CommandBufferState->BindVertexBufferIfNotBound(m_Square.GetBuffer());
		m_CommandBufferState->BindIndexBufferIfNotBound(m_Square.GetIndexBuffer(),
			VK_INDEX_TYPE_UINT32);

		float xOffset = 0.0f;
		float yOffset = std::numeric_limits<float>::min();
		switch (justify)
		{
			case TextJustify::CENTER:
			{
				for (size_t i = 0; i < text.size(); i++)
				{
					Font::Character& character = vkFont.GetCharacter(text[i]);

					float bearingY = (float)character.bearing.y * scale;

					if (bearingY > yOffset)
						yOffset = bearingY;

					xOffset -= (character.advance >> 6) * scale;
				}

				xOffset /= 2.0f;
				yOffset /= 2.0f;
			}
			break;

			case TextJustify::RIGHT:
			{
				for (size_t i = 0; i < text.size(); i++)
				{
					Font::Character& character = vkFont.GetCharacter(text[i]);
					xOffset -= (character.advance >> 6) * scale;
				}
			}
			break;
		}

		for (size_t i = 0; i < text.size(); i++)
		{
			Font::Character& character = vkFont.GetCharacter(text[i]);

			if (character.image)
				RenderCharacter(color, character, scale, x, y, xOffset, yOffset);

			xOffset += (character.advance >> 6) * scale;
		}
	}

	void VulkanRenderer::StartNewFrame(uint32_t commandBufferIndex, 
		VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent)
	{
		m_CommandBuffer = commandBuffer;
		m_CBufIndex = commandBufferIndex;
		m_SwapchainExtent = swapchainExtent;

		m_CommandBufferState.emplace(m_GraphicsContext.GetDevice(), 
			m_GraphicsContext.GetDeviceLoader(), m_CommandBuffer);
	}

	void VulkanRenderer::RenderCharacter(
		Math::Vector4f color,
		Font::Character& character,
		float scale, float x, float y,
		float xOffset, float yOffset
	)
	{
		float sizeX = (float)character.size.x * scale;
		float sizeY = (float)character.size.y * scale;
		float bearingX = (float)character.bearing.x * scale;
		float bearingY = (float)character.bearing.y * scale;

		TextPushConstants pushConstants;
		pushConstants.color = color;
		pushConstants.position.x = x + xOffset + bearingX;
		pushConstants.position.y = y + yOffset - bearingY;
		pushConstants.scale.x = sizeX;
		pushConstants.scale.y = sizeY;
		pushConstants.position.x /= m_SwapchainExtent.width;
		pushConstants.position.y /= m_SwapchainExtent.height;
		pushConstants.scale.x /= m_SwapchainExtent.width;
		pushConstants.scale.y /= m_SwapchainExtent.height;

		m_Dloader.vkCmdBindDescriptorSets(
			m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_TextPipeline.GetLayout(), 0,
			1, &character.descriptorSets[m_CBufIndex],
			0, nullptr
		);

		m_Dloader.vkCmdPushConstants(
			m_CommandBuffer, m_TextPipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(TextPushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdDrawIndexed(
			m_CommandBuffer,
			static_cast<uint32_t>(m_Square.GetVertexCount()),
			1, 0, 0, 0
		);
	}
}