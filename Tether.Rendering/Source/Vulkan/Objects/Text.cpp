#include <Tether/Module/Rendering/Vulkan/Objects/Text.hpp>

namespace Tether::Rendering::Vulkan
{
	Text::Text(Device& device, Pipeline& pipeline, VertexBuffer& rectBuffer,
		SimpleWindow& window)
		:
		Objects::Text(this),
		m_Device(device),
		m_Dloader(m_Device.GetLoader()),
		m_Pipeline(pipeline),
		m_RectBuffer(rectBuffer),
		m_Window(window)
	{

	}

	void Text::AddToCommandBuffer(CommandBufferDescriptor& commandBuffer,
		uint32_t index)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		commandBuffer.BindPipelineIfNotBound(&m_Pipeline);
		commandBuffer.BindVertexBufferIfNotBound(&m_RectBuffer);

		pushConstants.windowSize.x = m_Window.GetWidth();
		pushConstants.windowSize.y = m_Window.GetHeight();
		pushConstants.color = m_Color;
		
		Font* pFont = (Font*)m_pFont;
		
		float offset = 0.0f;
		for (size_t i = 0; i < m_Text.size(); i++)
		{
			Font::Character& character = pFont->GetCharacter(m_Text[i]);

			if (character.image)
				RenderCharacter(commandBuffer, index, character, offset);

			offset += (character.advance >> 6) * m_Scale;
		}
	}

	void Text::RenderCharacter(CommandBufferDescriptor& commandBuffer,
		uint32_t index, Font::Character& character, float offset)
	{
		VkCommandBuffer vkCommandBuffer = commandBuffer.Get();

		float startX = pushConstants.windowSize.x * x;
		float startY = pushConstants.windowSize.y * y;
		float bearingX = character.bearing.x * m_Scale;
		float bearingY = (float)character.size.y - character.bearing.y;
		float baseDistance = bearingY - character.size.y;

		pushConstants.position.x = startX + offset + bearingX;
		pushConstants.position.y = startY + baseDistance * m_Scale;
		pushConstants.scale.x = (float)character.size.x * m_Scale;
		pushConstants.scale.y = (float)character.size.y * m_Scale;

		m_Dloader->vkCmdBindDescriptorSets(
			vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_Pipeline.GetLayout(), 0,
			1, &character.descriptorSets[index],
			0, nullptr
		);

		m_Dloader->vkCmdPushConstants(
			vkCommandBuffer, m_Pipeline.GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PushConstants),
			&pushConstants
		);

		m_Dloader->vkCmdDrawIndexed(
			vkCommandBuffer,
			static_cast<uint32_t>(m_RectBuffer.GetVertexCount()),
			1, 0, 0, 0
		);
	}
}