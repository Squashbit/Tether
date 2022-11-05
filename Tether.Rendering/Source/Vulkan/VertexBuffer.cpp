#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

using namespace Tether::Rendering::Vulkan;

bool VertexBuffer::Init(VertexBufferInfo* pInfo)
{
	if (initialized)
		return false;

	this->info = *pInfo;

	initialized = true;
	return true;
}

bool VertexBuffer::UploadData(void* data, size_t dataSize, uint32_t* pIndices, 
	size_t indexCount)
{
	if (!initialized)
		return false;

	if (!UploadDataAsync(data, dataSize, pIndices, indexCount))
		return false;

	Wait();

	return true;
}

bool VertexBuffer::UploadDataAsync(void* data, size_t dataSize, uint32_t* pIndices, 
	size_t indexCount)
{
	if (!initialized)
		return false;

	uint32_t indexBufferSize = sizeof(uint32_t) * static_cast<uint32_t>(indexCount);

	Wait();
	if (!CreateVertexBuffer(static_cast<uint32_t>(dataSize)))
		return false;
	if (!CreateIndexBuffer(indexBufferSize))
	{
		vmaDestroyBuffer(info.allocator, vertexBuffer, vertexAllocation);
		return false;
	}
	
	this->vertexCount = indexCount;

	if (!UploadVertexData(data, dataSize) 
		|| !UploadIndexData(pIndices, indexBufferSize))
		return false;

	buffersCreated = true;
	return true;
}

bool VertexBuffer::UploadVertexData(void* data, size_t dataSize)
{
	BufferStagerInfo vertexInfo{};
	vertexInfo.allocator = info.allocator;
	vertexInfo.buffer = vertexBuffer;
	vertexInfo.bufferOwnerQueue = info.graphicsQueue;
	vertexInfo.device = info.device;
	vertexInfo.dloader = info.dloader;
	vertexInfo.pool = info.pool;

	if (!vertexStager.Init(&vertexInfo))
		return false;
	if (!vertexStager.UploadDataAsync(data, dataSize))
	{
		vertexStager.Wait();
		DestroyBuffers();

		vertexStager.Dispose();

		return false;
	}

	return true;
}

bool VertexBuffer::UploadIndexData(uint32_t* pIndices, size_t indexBufferSize)
{
	
	BufferStagerInfo indexInfo{};
	indexInfo.allocator = info.allocator;
	indexInfo.buffer = indexBuffer;
	indexInfo.bufferOwnerQueue = info.graphicsQueue;
	indexInfo.device = info.device;
	indexInfo.dloader = info.dloader;
	indexInfo.pool = info.pool;

	if (!indexStager.Init(&indexInfo))
		return false;
	if (!indexStager.UploadDataAsync(pIndices, indexBufferSize))
	{
		vertexStager.Wait();
		indexStager.Wait();
		DestroyBuffers();

		vertexStager.Dispose();
		indexStager.Dispose();

		return false;
	}

	return true;
}

void VertexBuffer::Wait()
{
	if (!buffersCreated || !initialized)
		return;

	vertexStager.Wait();
	indexStager.Wait();

	vertexStager.Dispose();
	indexStager.Dispose();

	buffersCreated = false;
}

size_t VertexBuffer::GetVertexCount()
{
	return vertexCount;
}

VkBuffer VertexBuffer::GetBuffer()
{
	Wait(); // Important
	return vertexBuffer;
}

VkBuffer VertexBuffer::GetIndexBuffer()
{
	Wait(); // Important
	return indexBuffer;
}

bool VertexBuffer::CreateVertexBuffer(uint32_t size)
{
	if (!initialized)
		return false;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = 
		  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

	return vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &vertexBuffer, &vertexAllocation, nullptr) == VK_SUCCESS;
}

bool VertexBuffer::CreateIndexBuffer(uint32_t size)
{
	if (!initialized)
		return false;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage =
		  VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

	return vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &indexBuffer, &indexAllocation, nullptr) == VK_SUCCESS;
}

void VertexBuffer::OnDispose()
{
	Wait();
	DestroyBuffers();
}

void VertexBuffer::DestroyBuffers()
{
	info.dloader->vkDeviceWaitIdle(info.device);

	vmaDestroyBuffer(info.allocator, vertexBuffer, vertexAllocation);
	vmaDestroyBuffer(info.allocator, indexBuffer, indexAllocation);
}