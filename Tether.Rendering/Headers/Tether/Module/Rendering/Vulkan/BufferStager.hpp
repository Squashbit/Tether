#pragma once

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	struct BufferStagerInfo
	{
		VmaAllocator allocator;
		VkDevice device;
		DeviceLoader* dloader;
		VkCommandPool pool;
		VkBuffer buffer;
		VkQueue bufferOwnerQueue;
	};

	class TETHER_EXPORT BufferStager : public IDisposable
	{
	public:
		BufferStager() = default;
		TETHER_DISPOSE_ON_DESTROY(BufferStager);
		TETHER_NO_COPY(BufferStager);

		bool Init(BufferStagerInfo* pInfo);

		// Data size is in bytes
		bool UploadData(void* data, size_t dataSize);
		// Data size is in bytes
		bool UploadDataAsync(void* data, size_t dataSize);

		// Wait for data to finish uploading.
		void Wait();
	private:
		void OnDispose();

		bool CreateCommandBuffer();
		bool CreateFence();

		bool CreateStagingBuffer(uint32_t uploadSize);
		bool RecordCommandBuffer(uint32_t uploadSize);
		void DestroyStagingBuffer();

		bool stagingBufferCreated = false;
		VkBuffer stagingBuffer;
		VmaAllocationInfo stagingInfo;
		VmaAllocation stagingAllocation;
		VkCommandBuffer commandBuffer;

		VkFence completedFence;

		BufferStagerInfo info;
	};
}