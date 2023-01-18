#include <Tether/Module/Rendering/Vulkan/VulkanBufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/SingleUseCommandBuffer.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

#include <cstring>

namespace Tether::Rendering::Vulkan
{
	VulkanBufferedImage::VulkanBufferedImage(
		Device* pDevice, VmaAllocator allocator,
		VkCommandPool commandPool, VkQueue graphicsQueue,
		const BufferedImageInfo& info
	)
		:
		pDevice(pDevice),
		dloader(pDevice->GetLoader()),
		allocator(allocator),
		commandPool(commandPool),
		graphicsQueue(graphicsQueue)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = info.width;
		imageInfo.extent.height = info.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT
			| VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		if (vmaCreateImage(allocator, &imageInfo, &allocInfo, 
			&image, &imageAllocation, nullptr) != VK_SUCCESS)
			throw RendererException("Failed to create image");

		UploadImageData(info);
		CreateImageView();
	}

	VulkanBufferedImage::~VulkanBufferedImage()
	{
		dloader->vkDestroyImageView(pDevice->Get(), imageView, nullptr);
		vmaDestroyImage(allocator, image, imageAllocation);
	}

	void VulkanBufferedImage::UploadImageData(const BufferedImageInfo& info)
	{
		VkBuffer stagingBuffer = nullptr;
		VmaAllocation stagingAllocation = nullptr;
		VmaAllocationInfo stagingInfo{};

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = info.width * info.height * 4;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &stagingBuffer, 
			&stagingAllocation, &stagingInfo) != VK_SUCCESS)
			throw RendererException("Failed to create staging buffer");

		memcpy(stagingInfo.pMappedData, info.pixelData, bufferInfo.size);

		SingleUseCommandBuffer singleUseCommandBuffer(pDevice, commandPool, 
			graphicsQueue);
		singleUseCommandBuffer.Begin();
		{
			singleUseCommandBuffer.TransitionImageLayout(
				image, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			);
			singleUseCommandBuffer.CopyBufferToImage(
				stagingBuffer, image,
				info.width, info.height
			);
			singleUseCommandBuffer.TransitionImageLayout(
				image, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);
		}
		singleUseCommandBuffer.Submit();

		vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
	}

	void VulkanBufferedImage::CreateImageView()
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (dloader->vkCreateImageView(pDevice->Get(), &viewInfo, nullptr, 
			&imageView) != VK_SUCCESS)
			throw RendererException("Failed to create texture image view");
	}
}