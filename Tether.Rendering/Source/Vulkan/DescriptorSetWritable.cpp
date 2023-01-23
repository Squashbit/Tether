#include <Tether/Module/Rendering/Vulkan/DescriptorSetWritable.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering::Vulkan
{
	VkDescriptorImageInfo DescriptorSetWritable::GetImageInfo(
		uint32_t setIndex)
	{
		throw RendererException("GetImageInfo called but was not implemented");
	}

	VkDescriptorBufferInfo DescriptorSetWritable::GetBufferInfo(
		uint32_t setIndex)
	{
		throw RendererException("GetBufferInfo called but was not implemented");
	}
}