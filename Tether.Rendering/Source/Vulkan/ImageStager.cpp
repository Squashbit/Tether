#include <Tether/Module/Rendering/Vulkan/ImageStager.hpp>
#include <stdexcept>
#include <cstring>

namespace Tether::Rendering::Vulkan
{
	ImageStager::ImageStager(
		VulkanContext& context,
		VkImage image, uint32_t imageWidth,
		uint32_t imageHeight, uint32_t bytesPerPixel, void* imageData,
		VkFormat imageFormat
	)
		:
		m_CommandBuffer(context),
		m_Allocator(context.allocator)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = imageWidth * imageHeight * bytesPerPixel;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &m_StagingBuffer,
			&m_StagingAllocation, &m_StagingInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to create staging buffer");

		memcpy(m_StagingInfo.pMappedData, imageData, bufferInfo.size);

		m_CommandBuffer.Begin();
		{
			m_CommandBuffer.TransitionImageLayout(
				image, imageFormat,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			);
			m_CommandBuffer.CopyBufferToImage(
				m_StagingBuffer, image,
				imageWidth, imageHeight
			);
			m_CommandBuffer.TransitionImageLayout(
				image, imageFormat,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);
		}
		m_CommandBuffer.End();
	}

	ImageStager::ImageStager(ImageStager&& other) noexcept
		:
		m_CommandBuffer(std::move(other.m_CommandBuffer)),
		m_StagingBuffer(other.m_StagingBuffer),
		m_StagingAllocation(other.m_StagingAllocation),
		m_StagingInfo(other.m_StagingInfo),
		m_Allocator(other.m_Allocator)
	{
		other.m_Moved = true;
	}

	ImageStager::~ImageStager()
	{
		if (m_Moved)
			return;

		vmaDestroyBuffer(m_Allocator, m_StagingBuffer, m_StagingAllocation);
	}

	void ImageStager::Upload(bool wait)
	{
		m_CommandBuffer.Submit(wait);
	}

	void ImageStager::Wait()
	{
		m_CommandBuffer.Wait();
	}
}