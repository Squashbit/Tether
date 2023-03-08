#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	DescriptorPool::DescriptorPool(VulkanContext& context, uint32_t maxSets,
		uint32_t sizeCount, VkDescriptorPoolSize* sizes)
		:
		m_Device(context.device),
		m_Dloader(context.deviceLoader)
	{
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = sizeCount;
		poolInfo.pPoolSizes = sizes;
		poolInfo.maxSets = maxSets;

		if (m_Dloader.vkCreateDescriptorPool(m_Device, &poolInfo, nullptr,
			&descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan descriptor pool");
	}

	DescriptorPool::~DescriptorPool()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);
		m_Dloader.vkDestroyDescriptorPool(m_Device, descriptorPool, nullptr);
	}

	VkDescriptorPool DescriptorPool::Get()
	{
		return descriptorPool;
	}
}