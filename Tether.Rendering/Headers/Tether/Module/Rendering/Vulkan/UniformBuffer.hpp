#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT UniformBuffer
	{
	public:
		UniformBuffer(
			VmaAllocator allocator, Device* pDevice, 
			DescriptorPool* pPool, VkDescriptorSetLayout layout,
			size_t bufferSize, uint32_t setCount
		);
		~UniformBuffer();
		TETHER_NO_COPY(UniformBuffer);

		void* GetMappedData(uint32_t index);
		VkDescriptorSet* GetSetAtIndex(uint32_t index);
	private:
		void CreateDescriptorSets(
			size_t bufferSize, 
			DescriptorPool* pPool, 
			VkDescriptorSetLayout layout
		);

		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VmaAllocation> uniformAllocations;
		std::vector<VmaAllocationInfo> uniformAllocInfos;

		VmaAllocator allocator = nullptr;
		Device* pDevice = nullptr;
		DeviceLoader* dloader = nullptr;
	};
}