#include <Tether/Module/Rendering/GraphicsContext.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	VulkanGraphicsContext::VulkanGraphicsContext(VulkanContext& context)
		:
		m_Context(context)
	{
		CreateVertexBuffers();
		CreateSampler();
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		m_Dloader.vkDestroySampler(m_Context.device, sampler, nullptr);
	}

	Scope<Objects::Rectangle> VulkanGraphicsContext::CreateRectangleObject()
	{
		return std::make_unique<Rectangle>(
			m_Context, square.value()
		);
	}

	Scope<Objects::Image> VulkanGraphicsContext::CreateImageObject()
	{
		return std::make_unique<Image>(
			m_Context, square.value()
		);
	}

	Scope<Objects::Text> VulkanGraphicsContext::CreateTextObject()
	{
		return std::make_unique<Text>(
			m_Context, square.value()
		);
	}

	Scope<Resources::BufferedImage> VulkanGraphicsContext::CreateBufferedImage(
		const Resources::BufferedImageInfo& info)
	{
		return std::make_unique<BufferedImage>(
			m_Context, sampler,
			texturedPipelineSetLayout,
			info
		);
	}

	Scope<Resources::Font> VulkanGraphicsContext::CreateFont(
		const std::string& fontPath)
	{
		return std::make_unique<Font>(
			m_Context,
			textPipelineLayout, sampler,
			fontPath
		);
	}

	void VulkanGraphicsContext::CreateVertexBuffers()
	{
		Math::Vector2f vertices[] =
		{
			{  0.0f,  0.0f},
			{  1.0f,  0.0f},
			{  1.0f,  1.0f},
			{  0.0f,  1.0f},
		};

		uint32_t indices[] =
		{
			0, 1, 2, 2, 3, 0
		};

		square.emplace(m_Context, sizeof(vertices),
			sizeof(indices) / sizeof(uint32_t));
		square->UploadData(vertices, indices);
		square->FinishDataUpload();
	}

	void VulkanGraphicsContext::CreateSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (m_Dloader.vkCreateSampler(m_Context.device, &samplerInfo, nullptr,
			&sampler) != VK_SUCCESS)
			throw std::runtime_error("Failed to create sampler");
	}
}