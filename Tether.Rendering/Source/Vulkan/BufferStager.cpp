#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <cstring>

using namespace Tether::Rendering::Vulkan;

BufferStager::BufferStager(
	VmaAllocator allocator,
	VkDevice device,
	DeviceLoader* dloader,
	VkCommandPool pool,
	VkQueue bufferOwnerQueue,
	VkBuffer buffer,
	size_t bufferSize
)
{
	TETHER_ASSERT(allocator != nullptr);
	TETHER_ASSERT(device != nullptr);
	TETHER_ASSERT(dloader != nullptr);
	TETHER_ASSERT(pool != nullptr);
	TETHER_ASSERT(buffer != nullptr);
	TETHER_ASSERT(bufferOwnerQueue != nullptr);

	this->allocator = allocator;
	this->device = device;
	this->dloader = dloader;
	this->pool = pool;
	this->bufferOwnerQueue = bufferOwnerQueue;
	this->buffer = buffer;
	this->bufferSize = bufferSize;

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
	memcpy(stagingInfo.pMappedData, data, bufferSize);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	dloader->vkResetFences(device, 1, &completedFence);
	dloader->vkQueueSubmit(bufferOwnerQueue, 1, &submitInfo, completedFence);
}

void BufferStager::Wait()
{
	dloader->vkWaitForFences(device, 1, &completedFence, true, UINT64_MAX);
}

void BufferStager::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pool;
	allocInfo.commandBufferCount = 1;

	if (dloader->vkAllocateCommandBuffers(device, &allocInfo, 
		&commandBuffer) != VK_SUCCESS)
		throw RendererException("Failed to create staging command buffer");
}

void BufferStager::CreateStagingBuffer()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	if (vmaCreateBuffer(allocator, &bufferInfo,
		&allocInfo, &stagingBuffer, &stagingAllocation, &stagingInfo) != VK_SUCCESS)
	{
		dloader->vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
		throw RendererException("Failed to create staging buffer");
	}
}

void BufferStager::CreateFence()
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (dloader->vkCreateFence(device, &fenceInfo, nullptr,
		&completedFence) != VK_SUCCESS)
	{
		dloader->vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
		throw RendererException("Failed to create staging buffer fence");
	}
}

bool BufferStager::RecordCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (dloader->vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return false;
	{
		VkBufferCopy copyRegion{};
		copyRegion.size = bufferSize;

		dloader->vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer,
			1, &copyRegion);
	}
	if (dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return false;

	return true;
}

void BufferStager::DisposeStager()
{
	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
	dloader->vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
	dloader->vkDestroyFence(device, completedFence, nullptr);
}

void BufferStager::OnDispose()
{
	Wait();
	DisposeStager();
}
