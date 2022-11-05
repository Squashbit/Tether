#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Tether::Rendering::Vulkan
{
    struct PipelineInfo
	{
		VkPipelineCreateFlags                            flags;
		uint32_t                                         stageCount;
		const VkPipelineShaderStageCreateInfo* pStages;
		const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
		const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
		const VkPipelineTessellationStateCreateInfo* pTessellationState;
		const VkPipelineViewportStateCreateInfo* pViewportState;
		const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
		const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
		const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
		const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
		const VkPipelineDynamicStateCreateInfo* pDynamicState;
		VkRenderPass                                     renderPass;
		uint32_t                                         subpass;
		VkPipeline                                       basePipelineHandle;
		int32_t                                          basePipelineIndex;
	};
}
