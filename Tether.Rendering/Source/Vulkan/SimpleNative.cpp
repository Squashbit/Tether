#include <Tether/Module/Rendering/Vulkan/SimpleNative.hpp>
#include <set>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/VkUtils.hpp>
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

#include <Tether/Module/Rendering/Common/VertexTypes.hpp>

#include <Tether.Rendering/Assets/CompiledShaders/solid.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/solid.frag.spv.h>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Vulkan;

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

ErrorCode SimpleNative::Init(SimpleWindow* pWindow)
{
	if (initialized)
		return ErrorCode::UNKNOWN;

	TETHER_TRY_INIT_APP_RETURNVAL(ErrorCode::APP_INIT_FAILED);

	RenderingModule& module = RenderingModule::Get();
	if (!module.IsVulkanInitialized())
		return ErrorCode::VULKAN_NOT_INITIALIZED;
	
	this->pWindow = pWindow;
	this->instance = &(module.GetVulkanNative()->instance);
	this->iloader = instance->GetLoader();

	if (!surface.Init(instance, pWindow))
		return ErrorCode::SURFACE_INIT_FAILED;

	if (!VkUtils::PickDevice(&physicalDevice, instance, &surface))
		return ErrorCode::DEVICE_NOT_FOUND;

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);

	if (!CreateDevice())
		return ErrorCode::DEVICE_INIT_FAILED;
	if (!CreateSwapchain())
		return ErrorCode::SWAPCHAIN_INIT_FAILED;
	if (!CreateRenderPass())
		return ErrorCode::RENDERPASS_INIT_FAILED;
	if (!CreateShaders())
		return ErrorCode::SHADER_INIT_FAILED;
	if (!CreatePipeline())
		return ErrorCode::PIPELINE_INIT_FAILED;
	if (!CreateFramebuffers())
		return ErrorCode::FRAMEBUFFER_INIT_FAILED;
	if (!CreateCommandPool())
		return ErrorCode::COMMANDPOOL_INIT_FAILED;
	if (!CreateCommandBuffer())
		return ErrorCode::COMMANDBUFFER_INIT_FAILED;
	if (!CreateSyncObjects())
		return ErrorCode::SYNC_OBJECT_INIT_FAILED;

	PopulateCommandBuffers();

	initialized = true;
	return ErrorCode::SUCCESS;
}

bool SimpleNative::RenderFrame()
{
	if (shouldRecreateSwapchain)
	{
		RecreateSwapchain();
		shouldRecreateSwapchain = false;
	}

	dloader->vkWaitForFences(device.Get(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex = 0;
	VkResult imageResult = dloader->vkAcquireNextImageKHR(device.Get(), 
		swapchain.Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame], 
		VK_NULL_HANDLE, &imageIndex);

	if (imageResult != VK_SUCCESS)
	{
		shouldRecreateSwapchain = true;
		return imageResult == VK_ERROR_OUT_OF_DATE_KHR || 
			   imageResult == VK_SUBOPTIMAL_KHR;
	}

	dloader->vkResetFences(device.Get(), 1, &inFlightFences[currentFrame]);

	if (shouldRecreateCommandBuffers)
	{
		PopulateCommandBuffers();
		shouldRecreateCommandBuffers = false;
	}

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (dloader->vkQueueSubmit(graphicsQueue, 1, &submitInfo, 
		inFlightFences[currentFrame])
			!= VK_SUCCESS)
		return false;

	VkSwapchainKHR swapchains[] = { swapchain.Get()};
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	if (dloader->vkQueuePresentKHR(presentQueue, &presentInfo) != VK_SUCCESS)
		return false;

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	return true;
}

bool SimpleNative::CreateDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		queueIndices.graphicsFamilyIndex,
		queueIndices.presentFamilyIndex
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures features{};

	if (!device.Init(
		instance,
		physicalDevice,
		queueCreateInfos.data(),
		static_cast<uint32_t>(queueCreateInfos.size()),
		features,
		deviceExtensions.data(),
		static_cast<uint32_t>(deviceExtensions.size())
	))
		return false;

	dloader = device.GetLoader();

	graphicsQueue = device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
	presentQueue = device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);

	return true;
}

bool SimpleNative::CreateSwapchain()
{
	Vulkan::SwapchainDetails details =
		instance->QuerySwapchainSupport(physicalDevice, &surface);
	VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details);
	uint32_t imageCount = FindImageCount(details);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.Get();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchain.ChooseExtent(details.capabilities,
		pWindow->GetWidth(), pWindow->GetHeight());
	createInfo.imageArrayLayers = 1;

	// This will probably be changed later on to take in a parameter in this
	// function.
	// This parameter in the swapchain specifies what the images in the
	// swapchain are used for.
	// This may seem familiar if you have done computer graphics before,
	// as this is only the color attachment. There are others too, such as
	// the depth buffer.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);
	if (queueIndices.graphicsFamilyIndex != queueIndices.presentFamilyIndex)
	{
		if (!queueIndices.hasPresentFamily)
			return false;

		uint32_t queueFamilyIndices[] =
		{
			queueIndices.graphicsFamilyIndex,
			queueIndices.presentFamilyIndex
		};

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (!swapchain.Init(&surface, &device, details,
		&createInfo))
		return false;

	swapchainImages = swapchain.GetImages();
	if (!swapchain.CreateImageViews(&swapchainImageViews))
		return false;

	return true;
}

bool SimpleNative::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain.GetImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] =
	{
		colorAttachment,
	};

	VkRenderPassCreateInfo desc{};
	desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	desc.attachmentCount = 1;
	desc.pAttachments = attachments;
	desc.subpassCount = 1;
	desc.pSubpasses = &subpass;
	desc.dependencyCount = 1;
	desc.pDependencies = &dependency;

	if (dloader->vkCreateRenderPass(device.Get(), &desc, nullptr, &renderPass)
		!= VK_SUCCESS)
		return false;

	return true;
}

bool SimpleNative::CreateShaders()
{
	if (!vertexModule.CreateFromSpirV(
		&device, Vulkan::ShaderType::VERTEX,
		(uint32_t*)Tether::Assets::VulkanShaders::_binary_solid_vert_spv,
		sizeof(Tether::Assets::VulkanShaders::_binary_solid_vert_spv)
	))
		return false;

	if (!fragmentModule.CreateFromSpirV(
		&device, ShaderType::FRAG,
		(uint32_t*)Tether::Assets::VulkanShaders::_binary_solid_frag_spv,
		sizeof(Tether::Assets::VulkanShaders::_binary_solid_frag_spv)
	))
		return false;

	return true;
}

bool SimpleNative::CreatePipeline()
{
	VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
	pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	
	if (dloader->vkCreatePipelineLayout(device.Get(), &pipelineLayoutDesc,
		nullptr, &pipelineLayout) != VK_SUCCESS)
		return false;
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkExtent2D swapchainExtent = swapchain.GetExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = swapchainExtent.width;
	scissor.extent.height = swapchainExtent.height;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

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

	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;

	// Oh, yes, cool thing about Vulkan, you can actually have multiple shader
	// stages for one shader module. That means that you can have a VSMain and
	// a PSMain in one shader module (aka a GLSL file in this case).

	VkPipelineShaderStageCreateInfo vertexStage{};
	vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStage.module = vertexModule.Get();
	vertexStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStage{};
	fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStage.module = fragmentModule.Get();
	fragmentStage.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] =
	{
		vertexStage, fragmentStage
	};

	VkGraphicsPipelineCreateInfo pipelineDesc{};
	pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineDesc.stageCount = sizeof(stages) / sizeof(stages[0]);
	pipelineDesc.pStages = stages;
	pipelineDesc.pVertexInputState = &vertexInputInfo;
	pipelineDesc.pInputAssemblyState = &inputAssembly;
	pipelineDesc.pViewportState = &viewportState;
	pipelineDesc.pRasterizationState = &rasterizer;
	pipelineDesc.pMultisampleState = &multisampleState;
	pipelineDesc.pColorBlendState = &colorBlending;
	pipelineDesc.layout = pipelineLayout;
	pipelineDesc.renderPass = renderPass;
	pipelineDesc.subpass = 0;
	pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;
	pipelineDesc.pDepthStencilState = nullptr;
	pipelineDesc.pDynamicState = &dynamicState;

	if (dloader->vkCreateGraphicsPipelines(device.Get(), VK_NULL_HANDLE,
		1, &pipelineDesc, nullptr, &pipeline) != VK_SUCCESS)
		return false;

	vertexModule.Dispose();
	fragmentModule.Dispose();

	return true;
}

bool SimpleNative::CreateFramebuffers()
{
	VkExtent2D swapchainExtent = swapchain.GetExtent();

	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		VkImageView attachments[] =
		{
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.width = swapchainExtent.width;
		createInfo.height = swapchainExtent.height;
		createInfo.layers = 1;

		if (dloader->vkCreateFramebuffer(device.Get(), &createInfo, nullptr,
			&swapchainFramebuffers[i]))
		{
			for (size_t i2 = 0; i2 < i + 1; i2++)
				dloader->vkDestroyFramebuffer(device.Get(), swapchainFramebuffers[i2],
					nullptr);

			return false;
		}
	}

	return true;
}

bool SimpleNative::CreateCommandPool()
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueIndices.graphicsFamilyIndex;

	return dloader->vkCreateCommandPool(device.Get(), &createInfo, nullptr,
		&commandPool) == VK_SUCCESS;
}

bool SimpleNative::CreateCommandBuffer()
{
	commandBuffers.resize(swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = swapchainFramebuffers.size();

	return dloader->vkAllocateCommandBuffers(device.Get(), &allocInfo, 
		commandBuffers.data()) == VK_SUCCESS;
}

bool SimpleNative::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (dloader->vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr,
			&imageAvailableSemaphores[i]) != VK_SUCCESS)
			return false;

		if (dloader->vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr,
			&renderFinishedSemaphores[i]) != VK_SUCCESS)
			return false;

		if (dloader->vkCreateFence(device.Get(), &fenceInfo, nullptr,
			&inFlightFences[i]) != VK_SUCCESS)
			return false;
	}

	return true;
}

bool SimpleNative::PopulateCommandBuffers()
{
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		dloader->vkResetCommandBuffer(commandBuffers[i], 0);
		if (!RecordCommandBuffer(commandBuffers[i], i))
			return false;
	}

	return true;
}

bool SimpleNative::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (dloader->vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		return false;
	{
		VkClearValue clearColor = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapchainFramebuffers[index];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain.GetExtent();
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		dloader->vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, 
			VK_SUBPASS_CONTENTS_INLINE);
		dloader->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipeline);

		VkExtent2D swapchainExtent = swapchain.GetExtent();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchainExtent.width;
		viewport.height = (float)swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		dloader->vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = swapchainExtent.width;
		scissor.extent.height = swapchainExtent.height;
		dloader->vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		
		dloader->vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		dloader->vkCmdEndRenderPass(commandBuffer);
	}
	if (dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return false;

	return true;
}

VkSurfaceFormatKHR SimpleNative::ChooseSurfaceFormat(SwapchainDetails details)
{
	for (const auto& availableFormat : details.formats)
		if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return details.formats[0];
}

uint32_t SimpleNative::FindImageCount(SwapchainDetails details)
{
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 &&
		imageCount > details.capabilities.maxImageCount)
		imageCount = details.capabilities.maxImageCount;

	return imageCount;
}

bool SimpleNative::RecreateSwapchain()
{
	device.WaitIdle();

	if (pWindow->GetWidth() == 0 || pWindow->GetHeight() == 0)
		return true;
	
	DestroySwapchain();

	if (!CreateSwapchain())
		return false;
	if (!CreateFramebuffers())
		return false;

	PopulateCommandBuffers();
}

void SimpleNative::DestroySwapchain()
{
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
		dloader->vkDestroyFramebuffer(device.Get(), swapchainFramebuffers[i],
			nullptr);

	for (VkImageView imageView : swapchainImageViews)
		dloader->vkDestroyImageView(device.Get(), imageView, nullptr);

	swapchain.Dispose();
}

void SimpleNative::OnDispose()
{
	device.WaitIdle();

	dloader->vkDestroyPipeline(device.Get(), pipeline, nullptr);
	dloader->vkDestroyPipelineLayout(device.Get(), pipelineLayout, nullptr);

	dloader->vkDestroyRenderPass(device.Get(), renderPass, nullptr);

	dloader->vkDestroyCommandPool(device.Get(), commandPool, nullptr);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		dloader->vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i], 
			nullptr);
		dloader->vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i], 
			nullptr);
		dloader->vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
	}

	DestroySwapchain();

	device.Dispose();
	surface.Dispose();
}