#include <Tether/Module/Rendering/Vulkan/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/ImageStager.hpp>
#include <stdexcept>

#include <cstring>

namespace Tether::Rendering::Vulkan
{
	BufferedImage::BufferedImage(
		VulkanContext& context,
		VkSampler sampler,
		VkDescriptorSetLayout pipelineSetLayout,
		const Resources::BufferedImageInfo& info
	)
		:
		Resources::BufferedImage(info),
		m_Context(context),
		m_Device(m_Context.device),
		m_Dloader(m_Context.deviceLoader),
		m_Sampler(sampler)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = info.width;
		imageInfo.extent.height = info.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_ImageFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT
			| VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(m_Context.allocator, &imageInfo, &allocInfo,
			&m_Image, &m_ImageAllocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image");

		UploadImageData(info);
		CreateImageView();

		VkDescriptorPoolSize samplerSize{};
		samplerSize.descriptorCount = m_Context.framesInFlight;
		samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		m_Pool.emplace(context, m_Context.framesInFlight, 1, &samplerSize);
		m_Set.emplace(*m_Pool, pipelineSetLayout, m_Context.framesInFlight);
		m_Set->UpdateSets(this, 0);
	}

	BufferedImage::~BufferedImage()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		m_Dloader.vkDestroyImageView(m_Device, m_ImageView, nullptr);
		vmaDestroyImage(m_Context.allocator, m_Image, m_ImageAllocation);
	}

	void BufferedImage::UploadImageData(const Resources::BufferedImageInfo& info)
	{
		ImageStager stager(
			m_Context, m_Image,
			info.width, info.height, 4, info.pixelData, m_ImageFormat
		);

		stager.Upload();
	}

	void BufferedImage::CreateImageView()
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_ImageFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (m_Dloader.vkCreateImageView(m_Device, &viewInfo, nullptr,
			&m_ImageView) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view");
	}

	VkDescriptorSet* BufferedImage::GetSetAtIndex(uint32_t index)
	{
		return m_Set->GetSetAtIndex(index);
	}

	VkDescriptorType BufferedImage::GetDescriptorType()
	{
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}

	VkDescriptorImageInfo BufferedImage::GetImageInfo(uint32_t setIndex)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_ImageView;
		imageInfo.sampler = m_Sampler;

		return imageInfo;
	}
}