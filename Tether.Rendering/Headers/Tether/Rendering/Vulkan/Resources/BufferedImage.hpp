#pragma once

#include <Tether/Rendering/Resources/BufferedImage.hpp>
#include <Tether/Rendering/Vulkan/Device.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSetWritable.hpp>

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vk_mem_alloc.h>
#include <optional>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT BufferedImage : public Resources::BufferedImage, 
		public DescriptorSetWritable
	{
	public:
		BufferedImage(
			GraphicsContext& context,
			VkSampler sampler,
			VkDescriptorSetLayout pipelineSetLayout,
			const Resources::BufferedImageInfo& info
		);
		~BufferedImage();

		VkDescriptorSet* GetSetAtIndex(uint32_t index);

		VkDescriptorType GetDescriptorType() override;
		VkDescriptorImageInfo GetImageInfo(uint32_t setIndex) override;
	private:
		void UploadImageData(GraphicsContext& context,
			const Resources::BufferedImageInfo& info);
		void CreateImageView();

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VmaAllocator m_Allocator = nullptr;
		VmaAllocation m_ImageAllocation = nullptr;

		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
		VkSampler m_Sampler = nullptr;

		std::optional<DescriptorPool> m_Pool;
		std::optional<DescriptorSet> m_Set;

		const VkFormat m_ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	};
}