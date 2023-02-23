#pragma once

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT BufferStager
	{
	public:
		BufferStager(
			Context& context,
			VkBuffer buffer,
			size_t bufferSize
		);
		~BufferStager();
		TETHER_NO_COPY(BufferStager);

		void UploadData(void* data);
		void UploadDataAsync(void* data);

		// Wait for data to finish uploading.
		void Wait();
	private:
		void CreateCommandBuffer();
		void CreateFence();

		void CreateStagingBuffer();
		void RecordCommandBuffer();

		void DisposeStager();

		VmaAllocationInfo m_StagingInfo;

		VkBuffer m_StagingBuffer = nullptr;
		VmaAllocation m_StagingAllocation = nullptr;
		VkCommandBuffer m_CommandBuffer = nullptr;
		VkFence m_CompletedFence = nullptr;

		Context& m_Context;

		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;
		
		VkBuffer m_Buffer = nullptr;
		size_t m_BufferSize = 0;
	};
}