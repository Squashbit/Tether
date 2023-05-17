#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/DescriptorPool.hpp>

#include <vector>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class DescriptorSetWritable;
	class TETHER_EXPORT DescriptorSet
	{
	public:
		DescriptorSet(
			DescriptorPool& pool,
			VkDescriptorSetLayout layout,
			uint32_t setCount,
			bool shouldFree = false
		);
		~DescriptorSet();
		TETHER_NO_COPY(DescriptorSet);

		void UpdateSets(DescriptorSetWritable* pWrite, uint32_t binding);

		uint32_t GetSetCount();
		VkDescriptorSet* GetSetAtIndex(uint32_t index);
	private:
		std::vector<VkDescriptorSet> m_DescriptorSets;

		bool m_ShouldFree = false;
		
		DescriptorPool& m_Pool;
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;
	};
}