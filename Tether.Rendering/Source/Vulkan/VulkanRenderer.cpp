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
	struct RectanglePushConstants
	{
		Math::Vector2f position;
		Math::Vector2f scale;
		Math::Vector4f color;
	};

	struct ImagePushConstants
	{
		Math::Vector2f position;
		Math::Vector2f scale;
	};

	VulkanRenderer::VulkanRenderer(GraphicsContext& graphicsContext)
		:
		m_GraphicsContext(graphicsContext),
		m_Square(m_GraphicsContext.GetSquareBuffer()),
		m_Dloader(m_GraphicsContext.GetDeviceLoader())
	{

	}

	void VulkanRenderer::FillRect(float x, float y, float width, float height, 
		Math::Vector4f color)
	{
		TETHER_THROW_IF_NO_COMMAND_BUFFER();

		RectanglePushConstants pushConstants;
		pushConstants.position.x = x / m_SwapchainExtent.width;
		pushConstants.position.y = y / m_SwapchainExtent.height;
		pushConstants.scale.x = width / m_SwapchainExtent.width;
		pushConstants.scale.y = height / m_SwapchainExtent.height;
		pushConstants.color = color;

		m_Dloader.vkCmdPushConstants(
			m_CommandBuffer, m_pRectPipeline->GetLayout(),
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

		m_Dloader.vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pImagePipeline->Get());

		VkBuffer vbuffers[] = { m_Square.GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		m_Dloader.vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, vbuffers, offsets);
		m_Dloader.vkCmdBindIndexBuffer(m_CommandBuffer, 
			m_Square.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

		m_Dloader.vkCmdPushConstants(
			m_CommandBuffer, m_pImagePipeline->GetLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(ImagePushConstants),
			&pushConstants
		);

		m_Dloader.vkCmdBindDescriptorSets(
			m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pImagePipeline->GetLayout(), 0,
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
	}

	void VulkanRenderer::StartNewFrame(uint32_t commandBufferIndex, 
		VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent)
	{
		m_CommandBuffer = commandBuffer;
		m_CBufIndex = commandBufferIndex;
		m_SwapchainExtent = swapchainExtent;
	}
}