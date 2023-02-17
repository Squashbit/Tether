#include <Tether/Module/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorSetWritable.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	DescriptorSet::DescriptorSet(
		DescriptorPool& pool, 
		VkDescriptorSetLayout layout, 
		uint32_t setCount,
		bool shouldFree
	)
		:
		m_Pool(pool),
		m_Device(m_Pool.m_Device),
		m_Dloader(m_Pool.m_Dloader),
		m_ShouldFree(shouldFree)
	{
		std::vector<VkDescriptorSetLayout> layouts(setCount, layout);
		m_DescriptorSets.resize(setCount);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_Pool.Get();
		allocInfo.descriptorSetCount = setCount;
		allocInfo.pSetLayouts = layouts.data();

		if (m_Dloader.vkAllocateDescriptorSets(m_Device, &allocInfo,
			m_DescriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate descriptor sets");
	}

	DescriptorSet::~DescriptorSet()
	{
		if (!m_ShouldFree)
			return;

		m_Dloader.vkFreeDescriptorSets(m_Device, m_Pool.Get(),
			static_cast<uint32_t>(m_DescriptorSets.size()), m_DescriptorSets.data());
	}

	void DescriptorSet::UpdateSets(DescriptorSetWritable* pWrite, uint32_t binding)
	{
		for (uint32_t i = 0; i < m_DescriptorSets.size(); i++)
		{
			VkDescriptorImageInfo imageInfo{};
			VkDescriptorBufferInfo bufferInfo{};

			VkWriteDescriptorSet writeInfo{};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.dstSet = m_DescriptorSets[i];
			writeInfo.dstBinding = binding;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorType = pWrite->GetDescriptorType();
			writeInfo.descriptorCount = 1;

			switch (writeInfo.descriptorType)
			{
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				{
					bufferInfo = pWrite->GetBufferInfo(i);
					writeInfo.pBufferInfo = &bufferInfo;
				}
				break;

				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				{
					imageInfo = pWrite->GetImageInfo(i);
					writeInfo.pImageInfo = &imageInfo;
				}
				break;
			}

			m_Dloader.vkUpdateDescriptorSets(m_Device, 1, &writeInfo, 0, 
				nullptr);
		}
	}

	uint32_t DescriptorSet::GetSetCount()
	{
		return static_cast<uint32_t>(m_DescriptorSets.size());
	}

	VkDescriptorSet* DescriptorSet::GetSetAtIndex(uint32_t index)
	{
		return &m_DescriptorSets[index];
	}
}