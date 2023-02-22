#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>

namespace Tether::Rendering::Vulkan
{
	VulkanCompositor::ResizeHandler::ResizeHandler(VulkanCompositor& renderer)
		:
		m_Renderer(renderer)
	{}

	void VulkanCompositor::ResizeHandler::OnWindowResize(
		Events::WindowResizeEvent event)
	{
		m_Renderer.RecreateSwapchain();
	}

	VulkanCompositor::VulkanCompositor(VulkanRenderer& renderer, Window& window, SimpleVulkanContext& context)
		:
		m_Renderer(renderer),
		m_Window(window),
		m_RenderPass(context, m_SurfaceFormat),
		m_Context(context),
		m_Surface(context, window),
		m_Dloader(m_Context.deviceLoader),
		m_ResizeHandler(*this)
	{
		Init();
	}

	void VulkanCompositor::Init()
	{
		CreateSwapchain();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandBuffers();

		m_Window.AddEventHandler(m_ResizeHandler, Events::EventType::WINDOW_RESIZE);
	}

	VulkanCompositor::~VulkanCompositor()
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

	bool VulkanCompositor::RenderFrame()
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

		if (m_Dloader.vkQueuePresentKHR(m_PresentQueue, &presentInfo)
			!= VK_SUCCESS)
			return false;

		m_CurrentFrame = (m_CurrentFrame + 1) % m_Context.framesInFlight;
		return true;
	}

	void VulkanCompositor::CreatePresentQueue()
	{
		uint32_t familyCount = 0;
		m_Context.instanceLoader.vkGetPhysicalDeviceQueueFamilyProperties(
			m_Context.physicalDevice, &familyCount, nullptr);

		if (familyCount == 0)
			return;

		std::vector<VkQueueFamilyProperties> families(familyCount);
		m_Context.instanceLoader.vkGetPhysicalDeviceQueueFamilyProperties(
			m_Context.physicalDevice, &familyCount, families.data());

		for (size_t i = 0; i < families.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = families[i];

			VkBool32 presentSupport = false;
			m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceSupportKHR(
				m_Context.physicalDevice, i, m_Surface.Get(), &presentSupport);
			if (presentSupport)
			{
				m_PresentFamilyIndex = i;

				m_Dloader.vkGetDeviceQueue(m_Context.device, m_PresentFamilyIndex, 1,
					&m_PresentQueue);

				return;
			}
		}

		throw std::runtime_error("Device has no present queue family");
	}

	void VulkanCompositor::CreateSwapchain()
	{	
		m_Swapchain.emplace(
			m_Context,
			m_Context.queueIndices.graphicsFamilyIndex, 
			m_PresentFamilyIndex,
			m_SwapchainDetails,
			m_SurfaceFormat,
			m_Surface.Get(),
			m_Window.GetWidth(), m_Window.GetHeight(),
			true
		);

		m_SwapchainImages = m_Swapchain->GetImages();
		m_Swapchain->CreateImageViews(&m_SwapchainImageViews);

		m_Renderer.SetSwapchainExtent(m_Swapchain->GetExtent());
	}

	void VulkanCompositor::CreateFramebuffers()
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

	void VulkanCompositor::CreateSyncObjects()
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

	void VulkanCompositor::CreateCommandBuffers()
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

	void VulkanCompositor::ChooseSurfaceFormat()
	{
		if (m_SwapchainDetails.formats.size() == 0)
			throw std::runtime_error("No available swapchain image formats");

		for (VkSurfaceFormatKHR availableFormat : m_SwapchainDetails.formats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
			{
				m_SurfaceFormat = availableFormat;
				return;
			}

		m_SurfaceFormat = m_SwapchainDetails.formats[0];
	}

	void VulkanCompositor::QuerySwapchainSupport()
	{
		VkSurfaceKHR vkSurface = m_Surface.Get();

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

	void VulkanCompositor::RecordCommandBuffer(VkCommandBuffer commandBuffer,
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

			m_Renderer.PopulateCommandBuffer(commandBuffer);

			m_Dloader.vkCmdEndRenderPass(commandBuffer);
		}
		if (m_Dloader.vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to end command buffer!");
	}

	bool VulkanCompositor::RecreateSwapchain()
	{
		if (m_Window.GetWidth() == 0 || m_Window.GetHeight() == 0)
			return true;

		DestroySwapchain();

		// The min and max window size fields in capabilities change on resize
		m_Context.instanceLoader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_Context.physicalDevice,
			m_Surface.Get(), &m_SwapchainDetails.capabilities
		);

		CreateSwapchain();
		CreateFramebuffers();

		return true;
	}

	void VulkanCompositor::DestroySwapchain()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
			m_Dloader.vkDestroyFramebuffer(m_Context.device, m_SwapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : m_SwapchainImageViews)
			m_Dloader.vkDestroyImageView(m_Context.device, imageView, nullptr);
	}
}