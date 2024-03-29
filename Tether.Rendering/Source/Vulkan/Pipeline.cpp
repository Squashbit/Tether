#include <Tether/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Rendering/Vulkan/ShaderModule.hpp>
#include <stdexcept>

using namespace Tether::Rendering::Vulkan;

PipelineLayout::PipelineLayout(GraphicsContext& context,
	VkPipelineLayoutCreateInfo* createInfo)
	:
	m_Device(context.GetDevice()),
	m_Dloader(context.GetDeviceLoader())
{
	if (m_Dloader.vkCreatePipelineLayout(m_Device, createInfo,
		nullptr, &layout) != VK_SUCCESS)
		throw std::runtime_error("Pipeline layout creation failed");
}

PipelineLayout::~PipelineLayout()
{
	m_Dloader.vkDestroyPipelineLayout(m_Device, layout, nullptr);
}

VkPipelineLayout PipelineLayout::Get()
{
	return layout;
}

Pipeline::Pipeline(
	GraphicsContext& context, VkRenderPass renderPass,
	VkExtent2D viewportExtent, uint32_t subpass,
	uint32_t* pVertexCode, size_t vertexCodeSize,
	uint32_t* pFragmentCode, size_t fragmentCodeSize,
	std::vector<VkVertexInputBindingDescription>& bindingDescs,
	std::vector<VkVertexInputAttributeDescription>& attribDescs,
	VkPipelineLayoutCreateInfo* customLayoutInfo
)
	:
	m_Device(context.GetDevice()),
	m_Dloader(context.GetDeviceLoader())
{
	if (customLayoutInfo != nullptr)
		layout.emplace(context, customLayoutInfo);
	else
	{
		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		layout.emplace(context, &layoutCreateInfo);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount =
		static_cast<uint32_t>(bindingDescs.size());
	vertexInputInfo.vertexAttributeDescriptionCount =
		static_cast<uint32_t>(attribDescs.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescs.data();
	vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

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
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	ShaderModule vertex(
		context,
		VK_SHADER_STAGE_VERTEX_BIT, 
		pVertexCode, vertexCodeSize
	);

	ShaderModule fragment(
		context,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		pFragmentCode, fragmentCodeSize
	);

	VkPipelineShaderStageCreateInfo stages[] =
	{
		vertex.GetStageInfo(),
		fragment.GetStageInfo(),
	};

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)viewportExtent.width;
	viewport.height = (float)viewportExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = viewportExtent.width;
	scissor.extent.height = viewportExtent.height;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.layout = layout->Get();
	createInfo.stageCount = sizeof(stages) / sizeof(VkPipelineShaderStageCreateInfo);
	createInfo.pStages = stages;
	createInfo.renderPass = renderPass;
	createInfo.subpass = subpass;
	createInfo.pViewportState = &viewportState;
	createInfo.pDynamicState = &dynamicState;
	createInfo.pVertexInputState = &vertexInputInfo;
	createInfo.pInputAssemblyState = &inputAssembly;
	createInfo.pRasterizationState = &rasterizer;
	createInfo.pMultisampleState = &multisampleState;
	createInfo.pColorBlendState = &colorBlending;
	createInfo.pDepthStencilState = nullptr;
	
	if (m_Dloader.vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE,
		1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
		throw std::runtime_error("Pipeline creation failed");
}

Pipeline::~Pipeline()
{
	m_Dloader.vkDestroyPipeline(m_Device, pipeline, nullptr);
}

VkPipeline Pipeline::Get()
{
	return pipeline;
}

VkPipelineLayout Pipeline::GetLayout()
{
	return layout->Get();
}
