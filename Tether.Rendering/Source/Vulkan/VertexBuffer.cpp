#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

using namespace Tether::Rendering::Vulkan;

void VertexBuffer::Init(VertexBufferInfo* pInfo, size_t dataSize, size_t indexCount)
{
	this->info = *pInfo;

	size_t indexBufferSize = sizeof(uint32_t) * indexCount;
	
	CreateVertexBuffer(dataSize);
	CreateIndexBuffer(indexBufferSize);
	
	BufferStagerInfo vertexInfo{};
	vertexInfo.allocator = info.allocator;
	vertexInfo.buffer = vertexBuffer;
	vertexInfo.bufferOwnerQueue = info.graphicsQueue;
	vertexInfo.device = info.device;
	vertexInfo.dloader = info.dloader;
	vertexInfo.pool = info.pool;
	vertexInfo.bufferSize = dataSize;
	
	BufferStagerInfo indexInfo{};
	indexInfo.allocator = info.allocator;
	indexInfo.buffer = indexBuffer;
	indexInfo.bufferOwnerQueue = info.graphicsQueue;
	indexInfo.device = info.device;
	indexInfo.dloader = info.dloader;
	indexInfo.pool = info.pool;
	indexInfo.bufferSize = indexBufferSize;

	vertexStager.Init(&vertexInfo);
	indexStager.Init(&indexInfo);

	this->vertexCount = indexCount;

	initialized = true;
}

void VertexBuffer::UploadData(void* data, uint32_t* pIndices)
{
	UploadDataAsync(data, pIndices);
	Wait();
}

void VertexBuffer::UploadDataAsync(void* data, uint32_t* pIndices)
{
	if (finishedUploading)
		return;

	vertexStager.UploadDataAsync(data);
	indexStager.UploadDataAsync(pIndices);
}

void VertexBuffer::Wait()
{
	if (finishedUploading)
		return;

	vertexStager.Wait();
	indexStager.Wait();
}

void VertexBuffer::FinishDataUpload()
{
	if (finishedUploading)
		return;

	Wait();

	vertexStager.Dispose();
	indexStager.Dispose();

	finishedUploading = true;
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

void VertexBuffer::CreateVertexBuffer(size_t size)
{
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

	if (vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &vertexBuffer, &vertexAllocation, nullptr) != VK_SUCCESS)
		throw RendererException("Failed to create vertex buffer");
}

void VertexBuffer::CreateIndexBuffer(size_t size)
{
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

	if (vmaCreateBuffer(info.allocator, &bufferInfo,
		&allocInfo, &indexBuffer, &indexAllocation, nullptr) != VK_SUCCESS)
	{
		vmaDestroyBuffer(info.allocator, vertexBuffer, vertexAllocation);
		throw RendererException("Failed to create index buffer");
	}
}

void VertexBuffer::OnDispose()
{
	Wait();
	DestroyBuffers();
}

void VertexBuffer::DestroyBuffers()
{
	info.dloader->vkDeviceWaitIdle(info.device);

	vertexStager.Dispose();
	indexStager.Dispose();

	vmaDestroyBuffer(info.allocator, vertexBuffer, vertexAllocation);
	vmaDestroyBuffer(info.allocator, indexBuffer, indexAllocation);
}