#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanBufferedImage.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/Rectangle.hpp>

#include <Tether.Rendering/Assets/CompiledShaders/solid.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/solid.frag.spv.h>

#include <set>

namespace Tether::Rendering::Vulkan
{
	VulkanRenderer::VulkanRenderer(SimpleWindow* pWindow)
		:
		pWindow(pWindow),
		instance(&RenderingModule::Get().GetVulkanNative()->instance.value()),
		iloader(instance->GetLoader()),
		surface(instance, pWindow),
		device(instance, surface.Get()),
		dloader(device.GetLoader()),
		allocator(instance->Get(), device.Get(), device.GetPhysicalDevice(), iloader),
		swapchainDetails(QuerySwapchainSupport()),
		surfaceFormat(ChooseSurfaceFormat()),
		renderPass(device.Get(), dloader, surfaceFormat.format)
	{
		queueIndices = instance->FindQueueFamilies(device.GetPhysicalDevice(),
			surface.Get());

		graphicsQueue = device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
		presentQueue = device.GetDeviceQueue(queueIndices.presentFamilyIndex, 0);

		CreateSwapchain();
		CreateShaders();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandPool();
		CreateVertexBuffers();
		CreateCommandBuffer();

		PopulateCommandBuffers();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		DestroySwapchain();

		dloader->vkDestroyCommandPool(device.Get(), commandPool, nullptr);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			dloader->vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i],
				nullptr);
			dloader->vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i],
				nullptr);
			dloader->vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
		}

		dloader->vkDestroyDescriptorSetLayout(device.Get(), descriptorSetLayout, 
			nullptr);
	}

	bool VulkanRenderer::RenderFrame()
	{
		if (shouldRecreateSwapchain)
		{
			RecreateSwapchain();
			shouldRecreateSwapchain = false;
		}

		dloader->vkWaitForFences(device.Get(), 1, &inFlightFences[currentFrame], 
			VK_TRUE, UINT64_MAX);

		uint32_t imageIndex = 0;
		VkResult imageResult = dloader->vkAcquireNextImageKHR(device.Get(),
			swapchain->Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame],
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

		VkSwapchainKHR swapchains[] = { swapchain->Get() };
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

	Scope<Rendering::BufferedImage> VulkanRenderer::CreateImage(
		const BufferedImageInfo& info)
	{
		return std::make_unique<VulkanBufferedImage>(&device, allocator.Get(), 
			commandPool, graphicsQueue, info);
	}

	void VulkanRenderer::WaitForCommandBuffers()
	{
		for (size_t i = 0; i < inFlightFences.size(); i++)
			dloader->vkWaitForFences(device.Get(), 1,
				&inFlightFences[i], VK_TRUE, UINT64_MAX);
	}

	uint32_t VulkanRenderer::GetSwapchainImageCount()
	{
		return swapchain->GetImageCount();
	}

	Device* VulkanRenderer::GetDevice()
	{
		return &device;
	}

	Pipeline* VulkanRenderer::GetPipeline()
	{
		return &pipeline.value();
	}

	VkDescriptorSetLayout VulkanRenderer::GetDescriptorSetLayout()
	{
		return descriptorSetLayout;
	}

	VertexBuffer* VulkanRenderer::GetRectangleBuffer()
	{
		return &square.value();
	}

	VmaAllocator VulkanRenderer::GetAllocator()
	{
		return allocator.Get();
	}

	void VulkanRenderer::OnCreateObject(Scope<Objects::Rectangle>& object)
	{
		object = std::make_unique<Rectangle>(this);
	}

	void VulkanRenderer::OnCreateObject(Scope<Objects::Image>& object)
	{
		object = std::make_unique<Image>(this);
	}

	void VulkanRenderer::OnObjectAdd(Objects::Object* pObject)
	{
		shouldRecreateCommandBuffers = true;
	}

	void VulkanRenderer::OnObjectRemove(Objects::Object* pObject)
	{
		shouldRecreateCommandBuffers = true;
	}

	void VulkanRenderer::CreateSwapchain()
	{
		swapchain.emplace(
			instance, &device,
			queueIndices, swapchainDetails, surfaceFormat, surface.Get(),
			pWindow->GetWidth(), pWindow->GetHeight(),
			true
		);

		swapchainImages = swapchain->GetImages();
		swapchain->CreateImageViews(&swapchainImageViews);
	}

	void VulkanRenderer::CreateShaders()
	{
		using namespace Assets;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT 
									| VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (dloader->vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr,
			&descriptorSetLayout) != VK_SUCCESS)
			throw RendererException("Failed to create descriptor set layout");

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vector2 descriptions
		{
			VkVertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.stride = sizeof(Math::Vector2f);
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescs.push_back(bindingDesc);

			VkVertexInputAttributeDescription posDesc;
			posDesc.binding = 0;
			posDesc.location = 0;
			posDesc.format = VK_FORMAT_R32G32_SFLOAT;
			posDesc.offset = 0;
			attribDescs.push_back(posDesc);
		}

		pipeline.emplace(
			&device, renderPass.Get(),
			swapchain->GetExtent(), 0,
			(uint32_t*)VulkanShaders::_binary_solid_vert_spv,
			sizeof(VulkanShaders::_binary_solid_vert_spv),
			(uint32_t*)VulkanShaders::_binary_solid_frag_spv,
			sizeof(VulkanShaders::_binary_solid_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void VulkanRenderer::CreateFramebuffers()
	{
		VkExtent2D swapchainExtent = swapchain->GetExtent();

		swapchainFramebuffers.resize(swapchainImageViews.size());

		for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
		{
			VkImageView attachments[] =
			{
				swapchainImageViews[i]
			};

			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = renderPass.Get();
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = swapchainExtent.width;
			createInfo.height = swapchainExtent.height;
			createInfo.layers = 1;

			if (dloader->vkCreateFramebuffer(device.Get(), &createInfo, nullptr,
				&swapchainFramebuffers[i]) != VK_SUCCESS)
			{
				for (size_t i2 = 0; i2 < i + 1; i2++)
					dloader->vkDestroyFramebuffer(device.Get(),
						swapchainFramebuffers[i2], nullptr);

				throw RendererException("Framebuffer creation failed");
			}
		}
	}

	void VulkanRenderer::CreateSyncObjects()
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
				throw RendererException("Semaphore creation failed");

			if (dloader->vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr,
				&renderFinishedSemaphores[i]) != VK_SUCCESS)
				throw RendererException("Semaphore creation failed");

			if (dloader->vkCreateFence(device.Get(), &fenceInfo, nullptr,
				&inFlightFences[i]) != VK_SUCCESS)
				throw RendererException("Fence creation failed");
		}
	}

	void VulkanRenderer::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = queueIndices.graphicsFamilyIndex;

		if (dloader->vkCreateCommandPool(device.Get(), &createInfo, nullptr,
			&commandPool) != VK_SUCCESS)
			throw RendererException("Command pool creation failed");
	}

	void VulkanRenderer::CreateCommandBuffer()
	{
		commandBuffers.resize(swapchainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (dloader->vkAllocateCommandBuffers(device.Get(), &allocInfo,
			commandBuffers.data()) != VK_SUCCESS)
			throw RendererException("Command buffer allocation failed");
	}

	void VulkanRenderer::CreateVertexBuffers()
	{
		Math::Vector2f vertices[] =
		{
			{  0.0f,  0.0f},
			{  1.0f,  0.0f},
			{  1.0f,  1.0f},
			{  0.0f,  1.0f},
		};

		uint32_t indices[] =
		{
			0, 1, 2, 2, 3, 0
		};

		VertexBufferInfo info{};
		info.allocator = allocator.Get();
		info.device = &device;
		info.graphicsQueue = graphicsQueue;
		info.pool = commandPool;

		square.emplace(&info, sizeof(vertices), sizeof(indices) / sizeof(uint32_t));
		square->UploadData(vertices, indices);
		square->FinishDataUpload();
	}

	VkSurfaceFormatKHR VulkanRenderer::ChooseSurfaceFormat()
	{
		if (swapchainDetails.formats.size() == 0)
			throw RendererException("No available swapchain image formats");

		for (VkSurfaceFormatKHR availableFormat : swapchainDetails.formats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
				&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		return swapchainDetails.formats[0];
	}

	SwapchainDetails VulkanRenderer::QuerySwapchainSupport()
	{
		VkPhysicalDevice physicalDevice = device.GetPhysicalDevice();
		VkSurfaceKHR surfacekhr = surface.Get();

		SwapchainDetails details;
		iloader->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, 
			surfacekhr, &details.capabilities);

		uint32_t formatCount;
		iloader->vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfacekhr, 
			&formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			iloader->vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, 
				surfacekhr, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		iloader->vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
			surfacekhr, &presentModeCount, details.presentModes.data());

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			iloader->vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 
				surfacekhr, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool VulkanRenderer::PopulateCommandBuffers()
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

	bool VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, 
		uint32_t index)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (dloader->vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			return false;
		{
			VkClearValue clearColor = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass.Get();
			renderPassInfo.framebuffer = swapchainFramebuffers[index];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchain->GetExtent();
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			dloader->vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);

			VkExtent2D swapchainExtent = swapchain->GetExtent();

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

			dloader->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->Get());

			AddObjectsToCommandBuffer(commandBuffer, index);

			dloader->vkCmdEndRenderPass(commandBuffer);
		}
		if (dloader->vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			return false;

		return true;
	}

	void VulkanRenderer::AddObjectsToCommandBuffer(VkCommandBuffer commandBuffer,
		uint32_t index)
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			Objects::Object* pObject = objects[i];
			Objects::ObjectRenderer* pObjectRenderer = pObject->GetObjectRenderer();
			if (!pObjectRenderer)
				continue;

			ObjectRenderer* pRenderer = (ObjectRenderer*)pObjectRenderer;

			pRenderer->AddToCommandBuffer(commandBuffer, index);
		}
	}

	bool VulkanRenderer::RecreateSwapchain()
	{
		if (pWindow->GetWidth() == 0 || pWindow->GetHeight() == 0)
			return true;

		DestroySwapchain();

		// The details contain the min and max width and height, and that changes 
		// every time the swapchain needs to be recreated.
		swapchainDetails = QuerySwapchainSupport();

		CreateSwapchain();
		CreateFramebuffers();

		PopulateCommandBuffers();

		return true;
	}

	void VulkanRenderer::DestroySwapchain()
	{
		device.WaitIdle();

		for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
			dloader->vkDestroyFramebuffer(device.Get(), swapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : swapchainImageViews)
			dloader->vkDestroyImageView(device.Get(), imageView, nullptr);
	}
}
