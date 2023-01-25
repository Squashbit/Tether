#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering::Vulkan
{
	DescriptorPool::DescriptorPool(Device* pDevice, uint32_t maxSets, 
		uint32_t sizeCount, VkDescriptorPoolSize* sizes)
	{
		this->pDevice = pDevice;
		this->dloader = pDevice->GetLoader();

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = sizeCount;
		poolInfo.pPoolSizes = sizes;
		poolInfo.maxSets = maxSets;

		if (dloader->vkCreateDescriptorPool(pDevice->Get(), &poolInfo, nullptr,
			&descriptorPool) != VK_SUCCESS)
			throw RendererException("Failed to create Vulkan descriptor pool");
	}

	DescriptorPool::~DescriptorPool()
	{
		pDevice->WaitIdle();
		dloader->vkDestroyDescriptorPool(pDevice->Get(), descriptorPool, nullptr);
	}

	VkDescriptorPool DescriptorPool::Get()
	{
		return descriptorPool;
	}
}