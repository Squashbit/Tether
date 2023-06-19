#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	SingleUseCommandBuffer::SingleUseCommandBuffer(GraphicsContext& context)
		:
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader()),
		m_CommandPool(context.GetCommandPool()),
		m_Queue(context.GetQueue())
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		m_Dloader.vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CommandBuffer);

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (m_Dloader.vkCreateFence(m_Device, &fenceInfo, nullptr, &m_Fence)
			!= VK_SUCCESS)
			throw std::runtime_error("Failed to create fence");
	}

	SingleUseCommandBuffer::SingleUseCommandBuffer(SingleUseCommandBuffer&& other) noexcept
		:
		m_CommandBuffer(other.m_CommandBuffer),
		m_Fence(other.m_Fence),
		m_Device(other.m_Device),
		m_Dloader(other.m_Dloader),
		m_CommandPool(other.m_CommandPool),
		m_Queue(other.m_Queue)
	{
		other.m_Moved = true;
	}

	SingleUseCommandBuffer::~SingleUseCommandBuffer()
	{
		if (m_Moved)
			return;

		m_Dloader.vkDestroyFence(m_Device, m_Fence, nullptr);
		m_Dloader.vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffer);
	}

	VkCommandBuffer SingleUseCommandBuffer::Begin()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		m_Dloader.vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);

		return m_CommandBuffer;
	}

	void SingleUseCommandBuffer::End()
	{
		m_Dloader.vkEndCommandBuffer(m_CommandBuffer);
	}

	void SingleUseCommandBuffer::Submit(bool wait)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		m_Dloader.vkQueueSubmit(m_Queue, 1, &submitInfo, m_Fence);

		if (wait)
			Wait();
	}

	void SingleUseCommandBuffer::Wait()
	{
		m_Dloader.vkWaitForFences(m_Device, 1, &m_Fence, VK_TRUE,
			UINT64_MAX);
	}

	void SingleUseCommandBuffer::CopyBufferToImage(
		VkBuffer buffer, VkImage image,
		uint32_t width, uint32_t height
	)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		m_Dloader.vkCmdCopyBufferToImage(
			m_CommandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	}

	void SingleUseCommandBuffer::TransitionImageLayout(
		VkImage image, VkFormat format,
		VkImageLayout oldLayout, VkImageLayout newLayout
	)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
			throw std::invalid_argument("Unsupported layout transition");

		m_Dloader.vkCmdPipelineBarrier(
			m_CommandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
}