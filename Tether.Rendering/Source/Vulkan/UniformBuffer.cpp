#include <Tether/Module/Rendering/Vulkan/UniformBuffer.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

namespace Tether::Rendering::Vulkan
{
	UniformBuffer::UniformBuffer(
		VmaAllocator allocator, Device* pDevice,
		DescriptorPool* pPool, VkDescriptorSetLayout layout,
		size_t bufferSize, uint32_t setCount
	)
	{
		this->allocator = allocator;
		this->pDevice = pDevice;
		this->dloader = pDevice->GetLoader();

		uniformBuffers.resize(setCount);
		uniformAllocations.resize(setCount);
		uniformAllocInfos.resize(setCount);

		for (size_t i = 0; i < setCount; i++)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = bufferSize;
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
			allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

			if (vmaCreateBuffer(
				allocator, &bufferInfo, &allocInfo,
				&uniformBuffers[i], &uniformAllocations[i],
				&uniformAllocInfos[i]) != VK_SUCCESS)
				throw RendererException("Failed to create staging buffer");
		}

		CreateDescriptorSets(bufferSize, pPool, layout);
	}

	void UniformBuffer::CreateDescriptorSets(
		size_t bufferSize,
		DescriptorPool* pPool,
		VkDescriptorSetLayout layout
	)
	{
		uint32_t setCount = static_cast<uint32_t>(uniformBuffers.size());

		std::vector<VkDescriptorSetLayout> layouts(setCount, layout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pPool->Get();
		allocInfo.descriptorSetCount = setCount;
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(layouts.size());
		if (dloader->vkAllocateDescriptorSets(pDevice->Get(), &allocInfo,
			descriptorSets.data()) != VK_SUCCESS)
			throw RendererException("Failed to allocate descriptor sets");

		for (size_t i = 0; i < layouts.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = bufferSize;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			dloader->vkUpdateDescriptorSets(pDevice->Get(), 1, &descriptorWrite, 
				0, nullptr);
		}
	}

	void* UniformBuffer::GetMappedData(uint32_t index)
	{
		return uniformAllocInfos[index].pMappedData;
	}

	VkDescriptorSet* UniformBuffer::GetSetAtIndex(uint32_t index)
	{
		return &descriptorSets[index];
	}

	UniformBuffer::~UniformBuffer()
	{
		pDevice->WaitIdle();

		for (size_t i = 0; i < uniformBuffers.size(); i++)
			vmaDestroyBuffer(allocator, uniformBuffers[i], uniformAllocations[i]);
	}
}