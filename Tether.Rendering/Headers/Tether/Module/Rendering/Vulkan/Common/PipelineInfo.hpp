#ifndef _TETHER_VULKAN_PIPELINEINFO_HPP
#define _TETHER_VULKAN_PIPELINEINFO_HPP

#include <Tether/Module/Rendering/Vulkan/UniformBuffer.hpp>

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Tether::Rendering::Vulkan
{
    struct PipelineInfo
	{
		uint32_t stageCount;
		VkPipelineShaderStageCreateInfo* pStages;
		uint32_t setCount;
		VkDescriptorSetLayout* pSetLayouts;
		uint32_t dynamicStateCount;
		VkDynamicState* pDynamicStates;
	};
}

#endif //_TETHER_VULKAN_PIPELINEINFO_HPP