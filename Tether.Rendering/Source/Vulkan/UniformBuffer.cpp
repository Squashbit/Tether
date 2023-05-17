#include <Tether/Rendering/Vulkan/UniformBuffer.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	UniformBuffer::UniformBuffer(GraphicsContext& context,
		size_t bufferSize, DescriptorSet& set, uint32_t binding)
		:
		m_BufferSize(bufferSize),
		m_Allocator(context.GetAllocator()),
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader())
	{
		uint32_t setCount = set.GetSetCount();

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
				m_Allocator, &bufferInfo, &allocInfo,
				&uniformBuffers[i], &uniformAllocations[i],
				&uniformAllocInfos[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create staging buffer");
		}

		set.UpdateSets(this, binding);
	}

	UniformBuffer::~UniformBuffer()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		for (size_t i = 0; i < uniformBuffers.size(); i++)
			vmaDestroyBuffer(m_Allocator, uniformBuffers[i], uniformAllocations[i]);
	}

	void* UniformBuffer::GetMappedData(uint32_t index)
	{
		return uniformAllocInfos[index].pMappedData;
	}

	VkDescriptorType UniformBuffer::GetDescriptorType()
	{
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	VkDescriptorBufferInfo UniformBuffer::GetBufferInfo(uint32_t setIndex)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[setIndex];
		bufferInfo.offset = 0;
		bufferInfo.range = m_BufferSize;

		return bufferInfo;
	}
}