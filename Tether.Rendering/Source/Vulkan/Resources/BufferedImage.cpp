#include <Tether/Module/Rendering/Vulkan/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/ImageStager.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

#include <cstring>

namespace Tether::Rendering::Vulkan
{
	BufferedImage::BufferedImage(
		Device& device, VmaAllocator allocator,
		VkCommandPool commandPool, VkQueue graphicsQueue,
		VkSampler sampler, uint32_t framesInFlight,
		VkDescriptorSetLayout pipelineSetLayout,
		const Resources::BufferedImageInfo& info
	)
		:
		Resources::BufferedImage(info),
		m_Device(device),
		m_Dloader(m_Device.GetLoader()),
		m_Allocator(allocator),
		m_CommandPool(commandPool),
		m_GraphicsQueue(graphicsQueue),
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

		if (vmaCreateImage(allocator, &imageInfo, &allocInfo, 
			&m_Image, &m_ImageAllocation, nullptr) != VK_SUCCESS)
			throw RendererException("Failed to create image");

		UploadImageData(info);
		CreateImageView();

		VkDescriptorPoolSize samplerSize{};
		samplerSize.descriptorCount = framesInFlight;
		samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		m_Pool.emplace(&m_Device, framesInFlight, 1, &samplerSize);
		m_Set.emplace(&m_Device, m_Pool.value(), pipelineSetLayout, framesInFlight);
		m_Set->UpdateSets(this, 0);
	}

	BufferedImage::~BufferedImage()
	{
		m_Dloader->vkDestroyImageView(m_Device.Get(), m_ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_Image, m_ImageAllocation);
	}

	void BufferedImage::UploadImageData(const Resources::BufferedImageInfo& info)
	{
		ImageStager stager(
			m_Device, m_CommandPool, m_GraphicsQueue,  m_Allocator, m_Image,
			info.width, info.height, 4, info.pixelData, VK_FORMAT_R8G8B8A8_UNORM
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

		if (m_Dloader->vkCreateImageView(m_Device.Get(), &viewInfo, nullptr,
			&m_ImageView) != VK_SUCCESS)
			throw RendererException("Failed to create texture image view");
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