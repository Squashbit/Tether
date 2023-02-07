#pragma once

#include <Tether/Module/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorSetWritable.hpp>

#include <vk_mem_alloc.h>
#include <optional>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT BufferedImage : public Resources::BufferedImage, 
		public DescriptorSetWritable
	{
	public:
		BufferedImage(
			Device& device, VmaAllocator allocator, 
			VkCommandPool commandPool, VkQueue graphicsQueue,
			VkSampler sampler, uint32_t framesInFlight,
			VkDescriptorSetLayout pipelineSetLayout,
			const Resources::BufferedImageInfo& info
		);
		~BufferedImage();

		VkDescriptorSet* GetSetAtIndex(uint32_t index);

		VkDescriptorType GetDescriptorType() override;
		VkDescriptorImageInfo GetImageInfo(uint32_t setIndex) override;
	private:
		void UploadImageData(const Resources::BufferedImageInfo& info);
		void CreateImageView();

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VmaAllocation m_ImageAllocation = nullptr;

		Device& m_Device;
		DeviceLoader* m_Dloader = nullptr;
		VmaAllocator m_Allocator = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VkQueue m_GraphicsQueue = nullptr;
		VkSampler m_Sampler = nullptr;

		std::optional<DescriptorPool> m_Pool;
		std::optional<DescriptorSet> m_Set;

		const VkFormat m_ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	};
}