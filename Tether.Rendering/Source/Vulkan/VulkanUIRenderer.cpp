#include <Tether/Module/Rendering/Vulkan/VulkanUIRenderer.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <set>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/VkUtils.hpp>
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

#include <Tether/Module/Rendering/Vulkan/ObjectNatives/VkObjectNative.hpp>
#include <Tether/Module/Rendering/Vulkan/ObjectNatives/RectangleNative.hpp>

#include <Tether/Module/Rendering/Common/VertexTypes.hpp>

#include <Tether.Rendering/Assets/CompiledShaders/solid.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/solid.frag.spv.h>

#include <Tether/Module/Rendering/Vulkan/Common/VulkanException.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Vulkan;

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanUIRenderer::VulkanUIRenderer(SimpleWindow* pWindow)
	:
	pWindow(pWindow)
{
	instance = &(RenderingModule::Get().GetVulkanNative()->instance);
	iloader = instance->GetLoader();

	if (!surface.Init(instance, pWindow))
		throw VulkanException("Surface creation failed");

	PickDevice();

	queueIndices = instance->FindQueueFamilies(physicalDevice, &surface);

	CreateDevice();
	CreateAllocator();
	CreateSwapchain();
	CreateRenderPass();
	CreatePipeline();
	CreateFramebuffers();
	CreateSyncObjects();
	CreateCommandPool();
	CreateVertexBuffers();
	CreateCommandBuffer();

	PopulateCommandBuffers();
}

bool VulkanUIRenderer::OnObjectCreate(HashedString& typeName, Objects::Object* pObject)
{
	Scope<Objects::ObjectNative> pNative;
	if (typeName == Objects::Rectangle::typeName)
		pNative = ScopeTools::Create<Natives::RectangleNative>(this);
	else
		return false;

	pObject->SetNative(pNative);
	return true;
}

void VulkanUIRenderer::OnObjectAdd(Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

void VulkanUIRenderer::OnObjectRemove(Objects::Object* pObject)
{
	shouldRecreateCommandBuffers = true;
}

bool VulkanUIRenderer::RenderFrame()
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

	if (shouldRecreateCommandBuffers)
	{
		PopulateCommandBuffers();
		shouldRecreateCommandBuffers = false;
	}

	dloader->vkResetFences(device.Get(), 1, &inFlightFences[currentFrame]);

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

void VulkanUIRenderer::CreateDevice()
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
		throw VulkanException("Device creation failed");

	dloader = device.GetLoader();

	graphicsQueue = device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
	presentQueue = device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);
}

void VulkanUIRenderer::CreateAllocator()
{
	VmaVulkanFunctions funcs{};
	funcs.vkGetInstanceProcAddr = TETHER_APP_VK->GetInstanceProcAddr;
	funcs.vkGetDeviceProcAddr = iloader->vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo createInfo{};
	createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	createInfo.physicalDevice = physicalDevice;
	createInfo.device = device.Get();
	createInfo.instance = instance->Get();
	createInfo.pVulkanFunctions = &funcs;

	if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS)
		throw VulkanException("VMA allocator creation failed");
}

void VulkanUIRenderer::CreateSwapchain()
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
			throw VulkanException("Device doesn't have a present family!");

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

	if (!swapchain.Init(&surface, &device, details, &createInfo))
		throw VulkanException("Swapchain creation failed");
	
	swapchainImages = swapchain.GetImages();
	if (!swapchain.CreateImageViews(&swapchainImageViews))
		throw VulkanException("Swapchain image view creation failed");
}

void VulkanUIRenderer::CreateRenderPass()
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
		throw VulkanException("Render pass creation failed");
}

void VulkanUIRenderer::CreatePipeline()
{
	using namespace Tether::Assets;

	VkShaderModule vertex;
	VkShaderModule fragment;

	VkShaderModuleCreateInfo vertexInfo{};
	vertexInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexInfo.pCode = (uint32_t*)VulkanShaders::_binary_solid_vert_spv;
	vertexInfo.codeSize = sizeof(VulkanShaders::_binary_solid_vert_spv);

	VkShaderModuleCreateInfo fragmentInfo{};
	fragmentInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentInfo.pCode = (uint32_t*)VulkanShaders::_binary_solid_frag_spv;
	fragmentInfo.codeSize = sizeof(VulkanShaders::_binary_solid_frag_spv);

	if (dloader->vkCreateShaderModule(device.Get(), &vertexInfo, nullptr,
		&vertex) != VK_SUCCESS)
		throw VulkanException("Vertex shader creation failed");
	if (dloader->vkCreateShaderModule(device.Get(), &fragmentInfo, nullptr,
		&fragment) != VK_SUCCESS)
		throw VulkanException("Fragment shader creation failed");

	std::vector<VkVertexInputBindingDescription> bindingDescs;
	std::vector<VkVertexInputAttributeDescription> attribDescs;

	// Vector2 descriptions
	{
		VkVertexInputBindingDescription bindingDesc;
		bindingDesc.binding = 0;
		bindingDesc.stride = sizeof(VertexTypes::Vertex2);
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescs.push_back(bindingDesc);

		VkVertexInputAttributeDescription posDesc;
		posDesc.binding = 0;
		posDesc.location = 0;
		posDesc.format = VK_FORMAT_R32G32_SFLOAT;
		posDesc.offset = offsetof(VertexTypes::Vertex2, position);
		attribDescs.push_back(posDesc);

		VkVertexInputAttributeDescription colorDesc;
		colorDesc.binding = 0;
		colorDesc.location = 1;
		colorDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorDesc.offset = offsetof(VertexTypes::Vertex2, color);
		attribDescs.push_back(colorDesc);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 
		static_cast<uint32_t>(bindingDescs.size());
	vertexInputInfo.vertexAttributeDescriptionCount = 
		static_cast<uint32_t>(attribDescs.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescs.data();
	vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

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
	vertexStage.module = vertex;
	vertexStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStage{};
	fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStage.module = fragment;
	fragmentStage.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] =
	{
		vertexStage, fragmentStage
	};

	PipelineInfo info{};
	info.stageCount = sizeof(stages) / sizeof(stages[0]);
	info.pStages = stages;
	info.pViewportState = &viewportState;
	info.renderPass = renderPass;
	info.pDynamicState = &dynamicState;
	info.pVertexInputState = &vertexInputInfo;

	if (!pipeline.Init(device.Get(), dloader, &info))
		throw VulkanException("Pipeline creation failed");
	
	dloader->vkDestroyShaderModule(device.Get(), vertex, nullptr);
	dloader->vkDestroyShaderModule(device.Get(), fragment, nullptr);
}

void VulkanUIRenderer::CreateFramebuffers()
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

			throw VulkanException("Framebuffer creation failed");
		}
	}
}

void VulkanUIRenderer::CreateCommandPool()
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueIndices.graphicsFamilyIndex;

	if (dloader->vkCreateCommandPool(device.Get(), &createInfo, nullptr,
		&commandPool) != VK_SUCCESS)
		throw VulkanException("Command pool creation failed");
}

void VulkanUIRenderer::CreateCommandBuffer()
{
	commandBuffers.resize(swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(swapchainFramebuffers.size());

	if (dloader->vkAllocateCommandBuffers(device.Get(), &allocInfo, 
		commandBuffers.data()) != VK_SUCCESS)
		throw VulkanException("Command buffer allocation failed");
}

void VulkanUIRenderer::CreateSyncObjects()
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
			throw VulkanException("Semaphore creation failed");

		if (dloader->vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr,
			&renderFinishedSemaphores[i]) != VK_SUCCESS)
			throw VulkanException("Semaphore creation failed");

		if (dloader->vkCreateFence(device.Get(), &fenceInfo, nullptr,
			&inFlightFences[i]) != VK_SUCCESS)
			throw VulkanException("Fence creation failed");
	}
}

void VulkanUIRenderer::CreateVertexBuffers()
{
	VertexTypes::Vertex2 vertices[] =
	{
		{ { -1.0f, -1.0f}, {0, 1, 1} },
		{ {  1.0f, -1.0f}, {1, 0, 1} },
		{ {  1.0f,  1.0f}, {0, 0, 0} },
		{ { -1.0f,  1.0f}, {1, 1, 0} },
	};

	uint32_t indices[] =
	{
		0, 1, 2, 2, 3, 0
	};

	VertexBufferInfo info{};
	info.allocator = allocator;
	info.device = device.Get();
	info.dloader = dloader;
	info.graphicsQueue = graphicsQueue;
	info.pool = commandPool;

	if (!square.Init(&info))
		throw VulkanException("Vertex buffer creation failed");

	square.UploadData(
		vertices, sizeof(vertices), 
		indices, sizeof(indices) / sizeof(uint32_t)
	);
}

VertexBuffer* VulkanUIRenderer::GetRectangleBuffer()
{
	return &square;
}

DeviceLoader* VulkanUIRenderer::GetDeviceLoader()
{
	return dloader;
}

bool VulkanUIRenderer::PopulateCommandBuffers()
{
	for (size_t i = 0; i < inFlightFences.size(); i++)
		dloader->vkWaitForFences(device.Get(), 1,
			&inFlightFences[i], VK_TRUE, UINT64_MAX);

	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		dloader->vkResetCommandBuffer(commandBuffers[i], 0);
		if (!RecordCommandBuffer(commandBuffers[i], static_cast<uint32_t>(i)))
			return false;
	}

	return true;
}

bool VulkanUIRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index)
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
			pipeline.Get());

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

		AddObjectsToCommandBuffer(commandBuffer, index);

		dloader->vkCmdEndRenderPass(commandBuffer);
	}
	if (dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		return false;

	return true;
}

void VulkanUIRenderer::AddObjectsToCommandBuffer(VkCommandBuffer commandBuffer,
	uint32_t index)
{
	using namespace Natives;
	
	for (size_t i = 0; i < objects.size(); i++)
	{
		Objects::Object* pObject = objects[i];
		VkObjectNative* pVkNative = (VkObjectNative*)pObject->GetNative();

		pVkNative->AddToCommandBuffer(commandBuffer, index);
	}
}

VkSurfaceFormatKHR VulkanUIRenderer::ChooseSurfaceFormat(SwapchainDetails details)
{
	for (VkSurfaceFormatKHR availableFormat : details.formats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
				&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return details.formats[0];
}

uint32_t VulkanUIRenderer::FindImageCount(SwapchainDetails details)
{
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 &&
		imageCount > details.capabilities.maxImageCount)
		imageCount = details.capabilities.maxImageCount;

	return imageCount;
}

bool VulkanUIRenderer::RecreateSwapchain()
{
	device.WaitIdle();

	if (pWindow->GetWidth() == 0 || pWindow->GetHeight() == 0)
		return true;
	
	DestroySwapchain();

	CreateSwapchain();
	CreateFramebuffers();

	PopulateCommandBuffers();

	return true;
}

void VulkanUIRenderer::DestroySwapchain()
{
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
		dloader->vkDestroyFramebuffer(device.Get(), swapchainFramebuffers[i],
			nullptr);

	for (VkImageView imageView : swapchainImageViews)
		dloader->vkDestroyImageView(device.Get(), imageView, nullptr);

	swapchain.Dispose();
}

bool VulkanUIRenderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	iloader->vkGetPhysicalDeviceProperties(device, &deviceProperties);
	iloader->vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices families =
		instance->FindQueueFamilies(device, &surface);

	bool extentionsSupported = instance->CheckDeviceExtentionSupport(device,
		deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		SwapchainDetails details = instance->QuerySwapchainSupport(device,
			&surface);
		swapChainGood = !details.formats.empty()
			&& !details.presentModes.empty();
	}

	return
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& families.hasGraphicsFamily
		&& families.hasPresentFamily
		&& swapChainGood
		&& extentionsSupported;
}

bool VulkanUIRenderer::PickDevice()
{
	uint32_t deviceCount = 0;
	iloader->vkEnumeratePhysicalDevices(instance->Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		return false;

	std::vector<VkPhysicalDevice> devices(deviceCount);
	iloader->vkEnumeratePhysicalDevices(instance->Get(), &deviceCount,
		devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			return true;
		}

	return false;
}

void VulkanUIRenderer::OnRendererDispose()
{
	device.WaitIdle();

	square.Dispose();

	pipeline.Dispose();

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

	vmaDestroyAllocator(allocator);

	device.Dispose();
	surface.Dispose();
}