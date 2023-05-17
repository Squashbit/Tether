#pragma once

#include <Tether/Rendering/Vulkan/SingleUseCommandBuffer.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ImageStager
	{
	public:
		ImageStager(
			VulkanContext& context,
			VkImage image, uint32_t imageWidth, 
			uint32_t imageHeight, uint32_t bytesPerPixel, void* imageData, 
			VkFormat imageFormat
		);
		ImageStager(ImageStager&& other) noexcept;
		~ImageStager();

		void Upload(bool wait = true);
		
		void Wait();
	private:
		bool m_Moved = false;

		SingleUseCommandBuffer m_CommandBuffer;

		VkBuffer m_StagingBuffer = nullptr;
		VmaAllocation m_StagingAllocation = nullptr;
		VmaAllocationInfo m_StagingInfo{};

		VmaAllocator m_Allocator = nullptr;
	};
}