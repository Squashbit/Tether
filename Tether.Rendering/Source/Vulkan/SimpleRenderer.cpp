#include <Tether/Module/Rendering/Vulkan/SimpleRenderer.hpp>

namespace Tether::Rendering::Vulkan
{
	SimpleRenderer::ResizeHandler::ResizeHandler(SimpleRenderer& renderer)
		:
		m_Renderer(renderer)
	{}

	void SimpleRenderer::ResizeHandler::OnWindowResize(
		Events::WindowResizeEvent event)
	{
		m_Renderer.RecreateSwapchain();
	}

	SimpleRenderer::SimpleRenderer(SimpleWindow& window, SimpleVulkanContext& context)
		:
		VulkanRenderer(context),
		m_Window(window),
		m_Context(context),
		m_Dloader(m_Context.deviceLoader),
		m_ResizeHandler(*this)
	{
		CreateSwapchain();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandBuffers();

		m_Window.AddEventHandler(m_ResizeHandler, Events::EventType::WINDOW_RESIZE);
	}

	SimpleRenderer::~SimpleRenderer()
	{
		DestroySwapchain();

		for (uint32_t i = 0; i < m_Context.framesInFlight; i++)
		{
			m_Dloader.vkDestroySemaphore(m_Context.device, m_ImageAvailableSemaphores[i],
				nullptr);
			m_Dloader.vkDestroySemaphore(m_Context.device, m_RenderFinishedSemaphores[i],
				nullptr);
			m_Dloader.vkDestroyFence(m_Context.device, m_InFlightFences[i], nullptr);
		}

		m_Window.RemoveEventHandler(m_ResizeHandler);
	}

	bool SimpleRenderer::RenderFrame()
	{
		if (m_ShouldRecreateSwapchain)
		{
			RecreateSwapchain();
			m_ShouldRecreateSwapchain = false;
		}

		m_Dloader.vkWaitForFences(m_Context.device, 1, &m_InFlightFences[m_CurrentFrame],
			VK_TRUE, UINT64_MAX);

		uint32_t imageIndex = 0;
		VkResult imageResult = m_Dloader.vkAcquireNextImageKHR(m_Context.device,
			m_Swapchain->Get(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame],
			VK_NULL_HANDLE, &imageIndex);

		if (imageResult != VK_SUCCESS)
		{
			m_ShouldRecreateSwapchain = true;
			return imageResult == VK_ERROR_OUT_OF_DATE_KHR ||
				imageResult == VK_SUBOPTIMAL_KHR;
		}

		RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

		m_Dloader.vkResetFences(m_Context.device, 1, &m_InFlightFences[m_CurrentFrame]);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (m_Dloader.vkQueueSubmit(m_Context.graphicsQueue, 1, &submitInfo,
			m_InFlightFences[m_CurrentFrame])
			!= VK_SUCCESS)
			return false;

		VkSwapchainKHR swapchains[] = { m_Swapchain->Get() };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		if (m_Dloader.vkQueuePresentKHR(m_Context.presentQueue, &presentInfo) 
			!= VK_SUCCESS)
			return false;

		m_CurrentFrame = (m_CurrentFrame + 1) % m_Context.framesInFlight;
		return true;
	}

	void SimpleRenderer::CreateSwapchain()
	{	
		m_Swapchain.emplace(
			m_Context,
			m_Context.queueIndices, 
			m_Context.swapchainDetails, 
			m_Context.surfaceFormat,
			m_Context.surface.Get(),
			m_Window.GetWidth(), m_Window.GetHeight(),
			true
		);

		m_SwapchainImages = m_Swapchain->GetImages();
		m_Swapchain->CreateImageViews(&m_SwapchainImageViews);

		SetSwapchainExtent(m_Swapchain->GetExtent());
	}

	void SimpleRenderer::CreateFramebuffers()
	{
		VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

		m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
		{
			VkImageView attachments[] =
			{
				m_SwapchainImageViews[i]
			};

			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = m_Context.renderPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = swapchainExtent.width;
			createInfo.height = swapchainExtent.height;
			createInfo.layers = 1;

			if (m_Dloader.vkCreateFramebuffer(m_Context.device, &createInfo, nullptr,
				&m_SwapchainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Framebuffer creation failed");
		}
	}

	void SimpleRenderer::CreateSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(m_Context.framesInFlight);
		m_RenderFinishedSemaphores.resize(m_Context.framesInFlight);
		m_InFlightFences.resize(m_Context.framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < m_Context.framesInFlight; i++)
		{
			if (m_Dloader.vkCreateSemaphore(m_Context.device, &semaphoreInfo, nullptr,
				&m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Semaphore creation failed");

			if (m_Dloader.vkCreateSemaphore(m_Context.device, &semaphoreInfo, nullptr,
				&m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Semaphore creation failed");

			if (m_Dloader.vkCreateFence(m_Context.device, &fenceInfo, nullptr,
				&m_InFlightFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Fence creation failed");
		}
	}

	void SimpleRenderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_Context.framesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Context.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (m_Dloader.vkAllocateCommandBuffers(m_Context.device, &allocInfo,
			m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Command buffer allocation failed");
	}

	void SimpleRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer,
		uint32_t imageIndex)
	{
		m_Dloader.vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (m_Dloader.vkBeginCommandBuffer(commandBuffer,
			&beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin command buffer!");
		{
			VkClearValue clearColor{};
			clearColor.color = { m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, 
				1.0f };

			VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_Context.renderPass;
			renderPassInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchainExtent;
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			m_Dloader.vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapchainExtent.width;
			viewport.height = (float)swapchainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			m_Dloader.vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			scissor.extent.width = swapchainExtent.width;
			scissor.extent.height = swapchainExtent.height;
			m_Dloader.vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			PopulateCommandBuffer(commandBuffer);

			m_Dloader.vkCmdEndRenderPass(commandBuffer);
		}
		if (m_Dloader.vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to end command buffer!");
	}

	bool SimpleRenderer::RecreateSwapchain()
	{
		if (m_Window.GetWidth() == 0 || m_Window.GetHeight() == 0)
			return true;

		DestroySwapchain();

		// The min and max window size fields in capabilities change on resize
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_Context.physicalDevice,
			m_Context.surface.Get(), &m_Context.swapchainDetails.capabilities
		);

		CreateSwapchain();
		CreateFramebuffers();

		return true;
	}

	void SimpleRenderer::DestroySwapchain()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
			m_Dloader.vkDestroyFramebuffer(m_Context.device, m_SwapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : m_SwapchainImageViews)
			m_Dloader.vkDestroyImageView(m_Context.device, imageView, nullptr);
	}
}