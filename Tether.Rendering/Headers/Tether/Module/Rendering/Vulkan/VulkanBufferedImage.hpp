#pragma once

#include <Tether/Module/Rendering/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanBufferedImage : public BufferedImage
	{
	public:
		VulkanBufferedImage(
			Device* pDevice, VmaAllocator allocator, 
			VkCommandPool commandPool, VkQueue graphicsQueue,
			const BufferedImageInfo& info
		);
		~VulkanBufferedImage();
	private:
		void UploadImageData(const BufferedImageInfo& info);
		void CreateImageView();

		VkImage image = nullptr;
		VkImageView imageView = nullptr;
		VmaAllocation imageAllocation = nullptr;

		Device* pDevice = nullptr;
		DeviceLoader* dloader = nullptr;
		VmaAllocator allocator = nullptr;
		VkCommandPool commandPool = nullptr;
		VkQueue graphicsQueue = nullptr;
	};
}