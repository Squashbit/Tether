#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <cstring>

using namespace Tether::Rendering::Vulkan;

bool BufferStager::Init(BufferStagerInfo* pInfo)
{
	if (initialized)
		return false;

	this->info = *pInfo;

	if (!CreateCommandBuffer())
		return false;

	if (!CreateFence())
	{
		info.dloader->vkFreeCommandBuffers(info.device, info.pool, 1, &commandBuffer);
		return false;
	}

	initialized = true;
	return true;
}

bool BufferStager::UploadData(void* data, size_t dataSize)
{
	if (!UploadDataAsync(data, dataSize))
		return false;

	Wait();

	return true;
}

bool BufferStager::UploadDataAsync(void* data, size_t dataSize)
{
	if (stagingBufferCreated)
		return false;

	uint32_t size32 = static_cast<uint32_t>(dataSize);

	if (!CreateStagingBuffer(size32))
		return false;

	memcpy(stagingInfo.pMappedData, data, size32);

	if (!RecordCommandBuffer(size32))
	{
		DestroyStagingBuffer();
		return false;
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	info.dloader->vkResetFences(info.device, 1, &completedFence);
	info.dloader->vkQueueSubmit(info.bufferOwnerQueue, 1, &submitInfo, completedFence);

	stagingBufferCreated = true;
	return true;
}

void BufferStager::Wait()
{
	info.dloader->vkWaitForFences(info.device, 1, &completedFence, true, UINT64_MAX);
}

void BufferStager::OnDispose()
{
	Wait();
	
	if (stagingBufferCreated)
	{
		DestroyStagingBuffer();
	}

	info.dloader->vkFreeCommandBuffers(info.device, info.pool, 1, &commandBuffer);
	info.dloader->vkDestroyFence(info.device, completedFence, nullptr);
}

bool BufferStager::CreateStagingBuffer(uint32_t uploadSize)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = uploadSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	return vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &stagingBuffer, &stagingAllocation, &stagingInfo) == VK_SUCCESS;
}

bool BufferStager::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = info.pool;
	allocInfo.commandBufferCount = 1;

	return info.dloader->vkAllocateCommandBuffers(info.device, &allocInfo, 
		&commandBuffer) == VK_SUCCESS;
}

bool BufferStager::CreateFence()
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	return info.dloader->vkCreateFence(info.device, &fenceInfo, nullptr,
		&completedFence) == VK_SUCCESS;
}

bool BufferStager::RecordCommandBuffer(uint32_t uploadSize)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (info.dloader->vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return false;
	{
		VkBufferCopy copyRegion{};
		copyRegion.size = uploadSize;

		info.dloader->vkCmdCopyBuffer(commandBuffer, stagingBuffer, info.buffer,
			1, &copyRegion);
	}
	if (info.dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return false;

	return true;
}

void BufferStager::DestroyStagingBuffer()
{
	vmaDestroyBuffer(info.allocator, stagingBuffer, stagingAllocation);
	stagingBufferCreated = false;
}
