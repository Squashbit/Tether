#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>
#include <stdexcept>

#include <cstring>

namespace Tether::Rendering::Vulkan
{
	BufferStager::BufferStager(
		VulkanContext& context,
		VkBuffer buffer,
		size_t bufferSize
	)
		:
		m_Context(context),
		m_Device(context.device),
		m_Dloader(context.deviceLoader),
		m_Buffer(buffer),
		m_BufferSize(bufferSize)
	{
		CreateCommandBuffer();
		CreateFence();
		CreateStagingBuffer();

		RecordCommandBuffer();
	}

	BufferStager::~BufferStager()
	{
		Wait();

		vmaDestroyBuffer(m_Context.allocator, m_StagingBuffer, m_StagingAllocation);
		m_Dloader.vkFreeCommandBuffers(m_Device, m_Context.commandPool, 1, &m_CommandBuffer);
		m_Dloader.vkDestroyFence(m_Device, m_CompletedFence, nullptr);
	}

	void BufferStager::UploadData(void* data)
	{
		UploadDataAsync(data);
		Wait();
	}

	void BufferStager::UploadDataAsync(void* data)
	{
		memcpy(m_StagingInfo.pMappedData, data, m_BufferSize);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		m_Dloader.vkResetFences(m_Device, 1, &m_CompletedFence);
		m_Dloader.vkQueueSubmit(m_Context.queue, 1, &submitInfo, 
			m_CompletedFence);
	}

	void BufferStager::Wait()
	{
		m_Dloader.vkWaitForFences(m_Device, 1, &m_CompletedFence, true, UINT64_MAX);
	}

	void BufferStager::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Context.commandPool;
		allocInfo.commandBufferCount = 1;

		if (m_Dloader.vkAllocateCommandBuffers(m_Device, &allocInfo,
			&m_CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create staging command buffer");
	}

	void BufferStager::CreateStagingBuffer()
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_BufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateBuffer(m_Context.allocator, &bufferInfo,
			&allocInfo, &m_StagingBuffer, &m_StagingAllocation, &m_StagingInfo) != VK_SUCCESS)
		{
			m_Dloader.vkFreeCommandBuffers(m_Device, m_Context.commandPool, 1, 
				&m_CommandBuffer);
			throw std::runtime_error("Failed to create staging buffer");
		}
	}

	void BufferStager::CreateFence()
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (m_Dloader.vkCreateFence(m_Device, &fenceInfo, nullptr,
			&m_CompletedFence) != VK_SUCCESS)
		{
			m_Dloader.vkFreeCommandBuffers(m_Device, m_Context.commandPool, 1, 
				&m_CommandBuffer);
			throw std::runtime_error("Failed to create staging buffer fence");
		}
	}

	void BufferStager::RecordCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (m_Dloader.vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to record staging command buffer");
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = m_BufferSize;

			m_Dloader.vkCmdCopyBuffer(m_CommandBuffer, m_StagingBuffer, m_Buffer,
				1, &copyRegion);
		}
		if (m_Dloader.vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record staging command buffer");
	}
}