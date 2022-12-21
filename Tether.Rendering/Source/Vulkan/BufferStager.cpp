#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <cstring>

using namespace Tether::Rendering::Vulkan;

void BufferStager::Init(BufferStagerInfo* pInfo)
{
	TETHER_ASSERT(pInfo != nullptr);

	this->info = *pInfo;

	CreateCommandBuffer();	
	CreateFence();
	CreateStagingBuffer();

	if (!RecordCommandBuffer())
	{
		DisposeStager();
		throw RendererException("Failed to record staging command buffer");
	}

	initialized = true;
}

void BufferStager::UploadData(void* data)
{
	UploadDataAsync(data);
	Wait();
}

void BufferStager::UploadDataAsync(void* data)
{
	memcpy(stagingInfo.pMappedData, data, info.bufferSize);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	info.dloader->vkResetFences(info.device, 1, &completedFence);
	info.dloader->vkQueueSubmit(info.bufferOwnerQueue, 1, &submitInfo, completedFence);
}

void BufferStager::Wait()
{
	info.dloader->vkWaitForFences(info.device, 1, &completedFence, true, UINT64_MAX);
}

void BufferStager::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = info.pool;
	allocInfo.commandBufferCount = 1;

	if (info.dloader->vkAllocateCommandBuffers(info.device, &allocInfo, 
		&commandBuffer) != VK_SUCCESS)
		throw RendererException("Failed to create staging command buffer");
}

void BufferStager::CreateStagingBuffer()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = info.bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	if (vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &stagingBuffer, &stagingAllocation, &stagingInfo) != VK_SUCCESS)
	{
		info.dloader->vkFreeCommandBuffers(info.device, info.pool, 1, &commandBuffer);
		throw RendererException("Failed to create staging buffer");
	}
}

void BufferStager::CreateFence()
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (info.dloader->vkCreateFence(info.device, &fenceInfo, nullptr,
		&completedFence) != VK_SUCCESS)
	{
		info.dloader->vkFreeCommandBuffers(info.device, info.pool, 1, &commandBuffer);
		throw RendererException("Failed to create staging buffer fence");
	}
}

bool BufferStager::RecordCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (info.dloader->vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return false;
	{
		VkBufferCopy copyRegion{};
		copyRegion.size = info.bufferSize;

		info.dloader->vkCmdCopyBuffer(commandBuffer, stagingBuffer, info.buffer,
			1, &copyRegion);
	}
	if (info.dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return false;

	return true;
}

void BufferStager::DisposeStager()
{
	vmaDestroyBuffer(info.allocator, stagingBuffer, stagingAllocation);
	info.dloader->vkFreeCommandBuffers(info.device, info.pool, 1, &commandBuffer);
	info.dloader->vkDestroyFence(info.device, completedFence, nullptr);
}

void BufferStager::OnDispose()
{
	Wait();
	DisposeStager();
}
