#include <Tether/Rendering/Vulkan/DescriptorSetWritable.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	VkDescriptorImageInfo DescriptorSetWritable::GetImageInfo(
		uint32_t setIndex)
	{
		throw std::runtime_error("GetImageInfo called but was not implemented");
	}

	VkDescriptorBufferInfo DescriptorSetWritable::GetBufferInfo(
		uint32_t setIndex)
	{
		throw std::runtime_error("GetBufferInfo called but was not implemented");
	}
}