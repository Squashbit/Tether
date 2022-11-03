#ifndef _TETHER_VULKAN_PIPELINEINFO_HPP
#define _TETHER_VULKAN_PIPELINEINFO_HPP

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

		uint32_t viewportCount;
		VkViewport* pViewports;
		uint32_t scissorCount;
		VkRect2D* pScissors;

		uint32_t vertexBindingDescriptionCount;
		VkVertexInputBindingDescription* pVertexBindingDescriptions;
		uint32_t vertexAttributeDescriptionCount;
		VkVertexInputAttributeDescription* pVertexAttributeDescriptions;

		VkDevice* pDevice;
		VkRenderPass* pRenderPass;
	};
}

#endif //_TETHER_VULKAN_PIPELINEINFO_HPP