#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>

using namespace Tether::Rendering::Vulkan;

void Pipeline::Init(VkDevice device, DeviceLoader* dloader, PipelineInfo* pInfo)
{
	TETHER_ASSERT(pInfo != nullptr);
	TETHER_ASSERT(dloader != nullptr);

	this->device = device;
	this->dloader = dloader;
	
	VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
	pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	if (dloader->vkCreatePipelineLayout(device, &pipelineLayoutDesc,
		nullptr, &pipelineLayout) != VK_SUCCESS)
		throw RendererException("Pipeline layout creation failed");

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleState{};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.minSampleShading = 0.0f;
	multisampleState.pSampleMask = nullptr;
	multisampleState.alphaToCoverageEnable = VK_FALSE;
	multisampleState.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	// Oh, yes, cool thing about Vulkan, you can actually have multiple shader
	// stages for one shader module. That means that you can have a VSMain and
	// a PSMain in one shader module (aka a GLSL file in this case).

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = pInfo->stageCount;
	createInfo.pStages = pInfo->pStages;
	createInfo.renderPass = pInfo->renderPass;
	createInfo.pViewportState = pInfo->pViewportState;
	createInfo.pDynamicState = pInfo->pDynamicState;
	createInfo.pDepthStencilState = pInfo->pDepthStencilState;
	createInfo.subpass = pInfo->subpass;
	createInfo.layout = pipelineLayout;

#define TETHER_PIPELINE_OVERRIDE_TEST(variable) \
	if (pInfo->variable) createInfo.variable = pInfo->variable

	createInfo.pVertexInputState = &vertexInputInfo;
	TETHER_PIPELINE_OVERRIDE_TEST(pVertexInputState);
	createInfo.pInputAssemblyState = &inputAssembly;
	TETHER_PIPELINE_OVERRIDE_TEST(pInputAssemblyState);
	createInfo.pRasterizationState = &rasterizer;
	TETHER_PIPELINE_OVERRIDE_TEST(pRasterizationState);
	createInfo.pMultisampleState = &multisampleState;
	TETHER_PIPELINE_OVERRIDE_TEST(pMultisampleState);
	createInfo.pColorBlendState = &colorBlending;
	TETHER_PIPELINE_OVERRIDE_TEST(pColorBlendState);
	
	if (dloader->vkCreateGraphicsPipelines(device, VK_NULL_HANDLE,
		1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
		throw RendererException("Pipeline creation failed");

	initialized = true;
}

VkPipeline Pipeline::Get()
{
	return pipeline;
}

VkPipelineLayout Pipeline::GetLayout()
{
	return pipelineLayout;
}

void Pipeline::OnDispose()
{
	dloader->vkDestroyPipeline(device, pipeline, nullptr);
	dloader->vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}