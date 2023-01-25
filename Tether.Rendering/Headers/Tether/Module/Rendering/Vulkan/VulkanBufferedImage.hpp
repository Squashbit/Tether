#pragma once

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorSetWritable.hpp>

#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanBufferedImage : public Resources::BufferedImage, 
		public DescriptorSetWritable
	{
	public:
		VulkanBufferedImage(
			Device* pDevice, VmaAllocator allocator, 
			VkCommandPool commandPool, VkQueue graphicsQueue,
			VkSampler sampler,
			const Resources::BufferedImageInfo& info
		);
		~VulkanBufferedImage();

		VkDescriptorType GetDescriptorType() override;
		VkDescriptorImageInfo GetImageInfo(uint32_t setIndex) override;
	private:
		void UploadImageData(const Resources::BufferedImageInfo& info);
		void CreateImageView();

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VmaAllocation m_ImageAllocation = nullptr;

		Device* m_pDevice = nullptr;
		DeviceLoader* m_Dloader = nullptr;
		VmaAllocator m_Allocator = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VkQueue m_GraphicsQueue = nullptr;
		VkSampler m_Sampler = nullptr;
	};
}