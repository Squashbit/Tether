#include <Tether/Module/Rendering/Vulkan/Objects/Text.hpp>
#include <limits>

namespace Tether::Rendering::Vulkan
{
	Text::Text(VulkanContext& context, VkExtent2D& swapchainExtent, 
		Pipeline& pipeline, VertexBuffer& rectBuffer)
		:
		Objects::Text((ObjectRenderer&)*this),
		m_Device(context.device),
		m_Dloader(context.deviceLoader),
		m_SwapchainExtent(swapchainExtent),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer)
	{

	}

	void Text::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		if (!m_pFont)
			return;

		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindIfNotBound(&m_Pipeline);
		commandBuffer.BindIfNotBound(&m_RectBuffer);

		pushConstants.color = m_Color;
		
		Font* pFont = (Font*)m_pFont;
		
		float xOffset = 0.0f;
		float yOffset = std::numeric_limits<float>::min();
		switch (m_Justify)
		{
			case Objects::Text::Justify::CENTER: 
			{
				for (size_t i = 0; i < m_Text.size(); i++)
				{
					Font::Character& character = pFont->GetCharacter(m_Text[i]);

					float bearingY = (float)character.bearing.y * m_Scale;

					if (bearingY > yOffset)
						yOffset = bearingY;

					xOffset -= (character.advance >> 6) * m_Scale;
				}

				xOffset /= 2.0f;
				yOffset /= 2.0f;
			}
			break;

			case Objects::Text::Justify::RIGHT: 
			{
				for (size_t i = 0; i < m_Text.size(); i++)
				{
					Font::Character& character = pFont->GetCharacter(m_Text[i]);
					xOffset -= (character.advance >> 6) * m_Scale;
				}
			}
			break;
		}

		for (size_t i = 0; i < m_Text.size(); i++)
		{
			Font::Character& character = pFont->GetCharacter(m_Text[i]);

			if (character.image)
				RenderCharacter(commandBuffer, index, character, xOffset, yOffset);

			xOffset += (character.advance >> 6) * m_Scale;
		}
	}

	void Text::RenderCharacter(
		CommandBufferDescriptor& commandBuffer,
		uint32_t index, Font::Character& character, float xOffset,
		float yOffset
	)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		float sizeX = (float)character.size.x * m_Scale;
		float sizeY = (float)character.size.y * m_Scale;
		float bearingX = (float)character.bearing.x * m_Scale;
		float bearingY = (float)character.bearing.y * m_Scale;
		
		pushConstants.position.x = x + xOffset + bearingX;
		pushConstants.position.y = y + yOffset - bearingY;
		pushConstants.scale.x = sizeX;
		pushConstants.scale.y = sizeY;

		pushConstants.position.x /= m_SwapchainExtent.width;
		pushConstants.position.y /= m_SwapchainExtent.height;
		pushConstants.scale.x /= m_SwapchainExtent.width;
		pushConstants.scale.y /= m_SwapchainExtent.height;

		m_Dloader.vkCmdBindDescriptorSets(
			vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_Pipeline.GetLayout(), 0,
			1, &character.descriptorSets[index],
			0, nullptr
		);

		m_Dloader.vkCmdPushConstants(
			vkCommandBuffer, m_Pipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_RectBuffer.GetVertexCount()),
			1, 0, 0, 0
		);
	}
}