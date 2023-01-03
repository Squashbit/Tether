#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <optional>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	struct VertexBufferInfo
	{
		VmaAllocator allocator = nullptr;
		VkDevice device = nullptr;
		DeviceLoader* dloader = nullptr;
		VkQueue graphicsQueue = nullptr;
		VkCommandPool pool = nullptr;
	};

	class TETHER_EXPORT VertexBuffer : public IDisposable
	{
	public:
		VertexBuffer(VertexBufferInfo* pInfo, size_t vertexBufferSize, size_t indexCount);
		TETHER_DISPOSE_ON_DESTROY(VertexBuffer);
		TETHER_NO_COPY(VertexBuffer);

		void UploadData(void* data, uint32_t* pIndices);
		void UploadDataAsync(void* data, uint32_t* pIndices);

		void Wait();
		
		// This function frees the staging buffers, so that less memory is used.
		// Once this is called, data can't be uploaded anymore.
		void FinishDataUpload();

		size_t GetVertexCount();
		VkBuffer GetBuffer();
		VkBuffer GetIndexBuffer();
	private:
		void OnDispose();

		void CreateVertexBuffer(size_t size);
		void CreateIndexBuffer(size_t size);
		
		void DestroyBuffers();

		size_t vertexCount = 0;

		bool finishedUploading = false;

		VkBuffer vertexBuffer;
		VmaAllocation vertexAllocation;
		VkBuffer indexBuffer;
		VmaAllocation indexAllocation;

		std::optional<BufferStager> vertexStager;
		std::optional<BufferStager> indexStager;
		
		VertexBufferInfo info;
	};
}