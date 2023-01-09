#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering::Vulkan
{
	DescriptorPool::DescriptorPool(Device* pDevice, uint32_t uniformDescriptorCount)
	{
		this->pDevice = pDevice;
		this->dloader = pDevice->GetLoader();

		VkDescriptorPoolSize uniformsPoolSize{};
		uniformsPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformsPoolSize.descriptorCount = uniformDescriptorCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &uniformsPoolSize;
		poolInfo.maxSets = uniformDescriptorCount;

		if (dloader->vkCreateDescriptorPool(pDevice->Get(), &poolInfo, nullptr,
			&descriptorPool) != VK_SUCCESS)
			throw RendererException("Failed to create Vulkan descriptor pool");
	}

	DescriptorPool::~DescriptorPool()
	{
		dloader->vkDestroyDescriptorPool(pDevice->Get(), descriptorPool, nullptr);
	}

	VkDescriptorPool DescriptorPool::Get()
	{
		return descriptorPool;
	}
}