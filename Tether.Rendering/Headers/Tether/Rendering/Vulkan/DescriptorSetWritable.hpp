#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT DescriptorSetWritable
	{
	public:
		virtual VkDescriptorType GetDescriptorType() = 0;
		
		virtual VkDescriptorImageInfo GetImageInfo(uint32_t setIndex);
		virtual VkDescriptorBufferInfo GetBufferInfo(uint32_t setIndex);
	};
}