#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Rendering/Vulkan/DescriptorPool.hpp>
#include <Tether/Rendering/Vulkan/Device.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSet.hpp>
#include <Tether/Rendering/Vulkan/DescriptorSetWritable.hpp>

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT UniformBuffer : public DescriptorSetWritable
	{
	public:
		UniformBuffer(GraphicsContext& context, size_t bufferSize,
			DescriptorSet& set, uint32_t binding);
		~UniformBuffer();
		TETHER_NO_COPY(UniformBuffer);

		void* GetMappedData(uint32_t index);

		VkDescriptorType GetDescriptorType() override;
		VkDescriptorBufferInfo GetBufferInfo(uint32_t setIndex) override;
	private:
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VmaAllocation> uniformAllocations;
		std::vector<VmaAllocationInfo> uniformAllocInfos;

		size_t m_BufferSize = 0;

		VmaAllocator m_Allocator = nullptr;
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
	};
}