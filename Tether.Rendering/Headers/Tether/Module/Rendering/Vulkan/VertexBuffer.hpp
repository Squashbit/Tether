#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/BufferStager.hpp>

#include <optional>

#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class GraphicsContext;
	class TETHER_EXPORT VertexBuffer
	{
	public:
		VertexBuffer(GraphicsContext& context, size_t vertexBufferSize,
			size_t indexCount);
		~VertexBuffer();
		TETHER_NO_COPY(VertexBuffer);

		void UploadData(void* data, uint32_t* pIndices);
		void UploadDataAsync(void* data, uint32_t* pIndices);

		void Wait();
		
		// This function frees the staging buffers, so that less memory is used.
		// Once this is called, data can't be uploaded anymore.
		void FinishDataUpload();

		size_t GetVertexCount() const;
		VkBuffer GetBuffer();
		VkBuffer GetIndexBuffer();
	private:
		void CreateVertexBuffer(size_t size);
		void CreateIndexBuffer(size_t size);
		
		void DestroyBuffers();

		size_t m_VertexCount = 0;

		bool m_FinishedUploading = false;

		VkBuffer m_VertexBuffer;
		VmaAllocation m_VertexAllocation;
		VkBuffer m_IndexBuffer;
		VmaAllocation m_IndexAllocation;

		std::optional<BufferStager> m_VertexStager;
		std::optional<BufferStager> m_IndexStager;
		
		GraphicsContext& m_Context;
	};
}