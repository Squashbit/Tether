#pragma once

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT BufferStager : public IDisposable
	{
	public:
		BufferStager(
			VmaAllocator allocator,
			VkDevice device,
			DeviceLoader* dloader,
			VkCommandPool pool,
			VkQueue bufferOwnerQueue,
			VkBuffer buffer,
			size_t bufferSize
		);
		TETHER_DISPOSE_ON_DESTROY(BufferStager);
		TETHER_NO_COPY(BufferStager);

		void UploadData(void* data);
		void UploadDataAsync(void* data);

		// Wait for data to finish uploading.
		void Wait();
	private:
		void OnDispose();

		void CreateCommandBuffer();
		void CreateFence();

		void CreateStagingBuffer();
		bool RecordCommandBuffer();

		void DisposeStager();

		VmaAllocationInfo stagingInfo;

		VkBuffer stagingBuffer = nullptr;
		VmaAllocation stagingAllocation = nullptr;
		VkCommandBuffer commandBuffer = nullptr;
		VkFence completedFence = nullptr;

		VkBuffer buffer = nullptr;
		size_t bufferSize = 0;

		VkDevice device = nullptr;
		VkQueue bufferOwnerQueue = nullptr;
		DeviceLoader* dloader = nullptr;

		VkCommandPool pool = nullptr;

		VmaAllocator allocator = nullptr;
	};
}