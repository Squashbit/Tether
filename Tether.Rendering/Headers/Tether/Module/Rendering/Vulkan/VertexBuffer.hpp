#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	struct VertexBufferInfo
	{
		VmaAllocator allocator;
		VkDevice device;
		DeviceLoader* dloader;
		VkQueue graphicsQueue;
		VkCommandPool pool;
	};

	class TETHER_EXPORT VertexBuffer : public IDisposable
	{
	public:
		VertexBuffer() = default;
		TETHER_DISPOSE_ON_DESTROY(VertexBuffer);
		TETHER_NO_COPY(VertexBuffer);

		bool Init(VertexBufferInfo* pInfo);

		// Data size is in bytes
		bool UploadData(void* data, size_t dataSize, uint32_t* pIndices, 
			size_t indexCount);
		// Data size is in bytes
		bool UploadDataAsync(void* data, size_t dataSize, uint32_t* pIndices, 
			size_t indexCount);

		// Waits for the vertex data to finish uploading
		void Wait();

		size_t GetVertexCount();
		VkBuffer GetBuffer();
		VkBuffer GetIndexBuffer();
	private:
		void OnDispose();

		bool CreateVertexBuffer(uint32_t size);
		bool CreateIndexBuffer(uint32_t size);
		bool UploadVertexData(void* data, size_t dataSize);
		bool UploadIndexData(uint32_t* pIndices, size_t indexBufferSize);

		void DestroyBuffers();

		size_t vertexCount = 0;

		bool buffersCreated = false;
		VkBuffer vertexBuffer;
		VmaAllocation vertexAllocation;
		VkBuffer indexBuffer;
		VmaAllocation indexAllocation;

		BufferStager vertexStager;
		BufferStager indexStager;
		
		VertexBufferInfo info;
	};
}