#include <Tether/Module/Rendering/Vulkan/Renderer.hpp>

namespace Tether::Rendering::Vulkan
{
	Renderer::ResizeHandler::ResizeHandler(Renderer& renderer)
		:
		m_Renderer(renderer)
	{}

	void Renderer::ResizeHandler::OnWindowResize(
		Events::WindowResizeEvent event)
	{
		m_Renderer.RecreateSwapchain();
	}

	Renderer::Renderer(Context& context, VulkanWindow& window)
		:
		m_Context(context),
		m_Window(window),
		m_Dloader(m_Context.deviceLoader),
		m_ResizeHandler(*this)
	{
		QuerySwapchainSupport();
		CheckPresentSupport();

		CreateSwapchain();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandBuffers();

		m_Window.window.AddEventHandler(m_ResizeHandler,
			Events::EventType::WINDOW_RESIZE);
	}

	Renderer::~Renderer()
	{
		DestroySwapchain();

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_Dloader.vkDestroySemaphore(m_Context.device, m_ImageAvailableSemaphores[i],
				nullptr);
			m_Dloader.vkDestroySemaphore(m_Context.device, m_RenderFinishedSemaphores[i],
				nullptr);
			m_Dloader.vkDestroyFence(m_Context.device, m_InFlightFences[i], nullptr);
		}

		m_Window.window.RemoveEventHandler(m_ResizeHandler);
	}

	bool Renderer::RenderFrame()
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

		if (m_Dloader.vkQueueSubmit(m_Context.queue, 1, &submitInfo,
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

		if (m_Dloader.vkQueuePresentKHR(m_Context.queue, &presentInfo)
			!= VK_SUCCESS)
			return false;

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		return true;
	}

	uint32_t Renderer::GetSwapchainImageCount()
	{
		return m_Swapchain->GetImageCount();
	}

	void Renderer::CheckPresentSupport()
	{
		VkBool32 presentSupport = false;
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceSupportKHR(
			m_Context.physicalDevice, m_Context.indices.graphicsFamilyIndex, 
			m_Window.m_Surface.Get(), &presentSupport
		);

		if (!presentSupport)
			throw std::runtime_error("Device has no present queue family");
	}

	void Renderer::CreateSwapchain()
	{	
		m_Swapchain.emplace(
			m_Context,
			m_Context.indices.graphicsFamilyIndex,
			m_SwapchainDetails,
			m_Window.m_SurfaceFormat,
			m_Window.m_Surface.Get(),
			m_Window.window.GetWidth(),
			m_Window.window.GetHeight(),
			true
		);

		m_SwapchainImages = m_Swapchain->GetImages();
		m_Swapchain->CreateImageViews(&m_SwapchainImageViews);
	}

	void Renderer::CreateFramebuffers()
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
			createInfo.renderPass = m_Window.renderPass;
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

	void Renderer::CreateSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
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

	void Renderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Context.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (m_Dloader.vkAllocateCommandBuffers(m_Context.device, &allocInfo,
			m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Command buffer allocation failed");
	}

	void Renderer::QuerySwapchainSupport()
	{
		VkSurfaceKHR vkSurface = m_Window.m_Surface.Get();

		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_Context.physicalDevice,
			vkSurface, &m_SwapchainDetails.capabilities
		);

		uint32_t formatCount;
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			m_Context.physicalDevice, vkSurface,
			&formatCount, nullptr
		);

		if (formatCount != 0)
		{
			m_SwapchainDetails.formats.resize(formatCount);
			m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceFormatsKHR(
				m_Context.physicalDevice,
				vkSurface, &formatCount,
				m_SwapchainDetails.formats.data()
			);
		}

		uint32_t presentModeCount;
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfacePresentModesKHR(
			m_Context.physicalDevice,
			vkSurface, &presentModeCount,
			m_SwapchainDetails.presentModes.data()
		);

		if (presentModeCount != 0)
		{
			m_SwapchainDetails.presentModes.resize(presentModeCount);
			m_Context.instanceLoader.vkGetPhysicalDeviceSurfacePresentModesKHR(
				m_Context.physicalDevice,
				vkSurface, &presentModeCount,
				m_SwapchainDetails.presentModes.data()
			);
		}
	}

	void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer,
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
			clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };

			VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_Window.renderPass;
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

			m_Dloader.vkCmdEndRenderPass(commandBuffer);
		}
		if (m_Dloader.vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to end command buffer!");
	}

	bool Renderer::RecreateSwapchain()
	{
		if (m_Window.window.GetWidth() == 0 || m_Window.window.GetHeight() == 0)
			return true;

		DestroySwapchain();

		// The min and max window size fields in capabilities change on resize
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_Context.physicalDevice,
			m_Window.m_Surface.Get(), &m_SwapchainDetails.capabilities
		);

		CreateSwapchain();
		CreateFramebuffers();

		return true;
	}

	void Renderer::DestroySwapchain()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
			m_Dloader.vkDestroyFramebuffer(m_Context.device, m_SwapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : m_SwapchainImageViews)
			m_Dloader.vkDestroyImageView(m_Context.device, imageView, nullptr);
	}
}