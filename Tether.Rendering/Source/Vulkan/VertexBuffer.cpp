#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/GraphicsContext.hpp>

#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	VertexBuffer::VertexBuffer(GraphicsContext& context,
		size_t vertexBufferSize, size_t indexCount)
		:
		m_Context(context)
	{
		size_t indexBufferSize = sizeof(uint32_t) * indexCount;

		CreateVertexBuffer(vertexBufferSize);
		CreateIndexBuffer(indexBufferSize);

		m_VertexStager.emplace(
			context,
			m_VertexBuffer,
			vertexBufferSize
		);

		m_IndexStager.emplace(
			context,
			m_IndexBuffer,
			indexBufferSize
		);

		this->m_VertexCount = indexCount;
	}

	VertexBuffer::~VertexBuffer()
	{
		Wait();
		DestroyBuffers();
	}

	void VertexBuffer::UploadData(void* data, uint32_t* pIndices)
	{
		UploadDataAsync(data, pIndices);
		Wait();
	}

	void VertexBuffer::UploadDataAsync(void* data, uint32_t* pIndices)
	{
		if (m_FinishedUploading)
			return;

		m_VertexStager->UploadDataAsync(data);
		m_IndexStager->UploadDataAsync(pIndices);
	}

	void VertexBuffer::Wait()
	{
		if (m_FinishedUploading)
			return;

		m_VertexStager->Wait();
		m_IndexStager->Wait();
	}

	void VertexBuffer::FinishDataUpload()
	{
		if (m_FinishedUploading)
			return;

		Wait();

		m_VertexStager.reset();
		m_IndexStager.reset();

		m_FinishedUploading = true;
	}

	size_t VertexBuffer::GetVertexCount() const
	{
		return m_VertexCount;
	}

	VkBuffer VertexBuffer::GetBuffer()
	{
		Wait(); // Important
		return m_VertexBuffer;
	}

	VkBuffer VertexBuffer::GetIndexBuffer()
	{
		Wait(); // Important
		return m_IndexBuffer;
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

		if (vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo,
			&allocInfo, &m_VertexBuffer, &m_VertexAllocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer");
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

		if (vmaCreateBuffer(m_Context.GetAllocator(), &bufferInfo,
			&allocInfo, &m_IndexBuffer, &m_IndexAllocation, nullptr) != VK_SUCCESS)
		{
			vmaDestroyBuffer(m_Context.GetAllocator(), m_VertexBuffer, m_VertexAllocation);
			throw std::runtime_error("Failed to create index buffer");
		}
	}

	void VertexBuffer::DestroyBuffers()
	{
		m_Context.GetDeviceLoader().vkDeviceWaitIdle(m_Context.GetDevice());

		m_VertexStager.reset();
		m_IndexStager.reset();

		vmaDestroyBuffer(m_Context.GetAllocator(), m_VertexBuffer, m_VertexAllocation);
		vmaDestroyBuffer(m_Context.GetAllocator(), m_IndexBuffer, m_IndexAllocation);
	}
}
