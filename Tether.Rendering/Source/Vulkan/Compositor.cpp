#include <Tether/Module/Rendering/Vulkan/Compositor.hpp>
#include <Tether/Module/Rendering/Vulkan/Renderer.hpp>

namespace Tether::Rendering::Vulkan
{
	Compositor::ResizeHandler::ResizeHandler(Compositor& renderer)
		:
		m_Renderer(renderer)
	{}

	void Compositor::ResizeHandler::OnWindowResize(
		const Events::WindowResizeEvent& event)
	{
		if (m_Renderer.m_AutoRecreateSwapchain)
			m_Renderer.RecreateSwapchain();
	}

	Compositor::Compositor(GraphicsContext& context, Window& window,
		bool autoRecreateSwapchain)
		:
		m_Context(context),
		m_Window(window),
		m_Surface(context, window),
		m_FramesInFlight(m_FramesInFlight),
		m_Device(m_Context.GetDevice()),
		m_Dloader(m_Context.GetDeviceLoader()),
		m_AutoRecreateSwapchain(autoRecreateSwapchain),
		m_ResizeHandler(*this)
	{
		Init();
	}

	void Compositor::Init()
	{
		QuerySwapchainSupport();
		CheckPresentSupport();

		CreateSwapchain();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandBuffers();

		m_Window.AddEventHandler(m_ResizeHandler, 
			Events::EventType::WINDOW_RESIZE);
	}

	Compositor::~Compositor()
	{
		DestroySwapchain();

		for (uint32_t i = 0; i < m_FramesInFlight; i++)
		{
			m_Dloader.vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i],
				nullptr);
			m_Dloader.vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i],
				nullptr);
			m_Dloader.vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
		}

		m_Window.RemoveEventHandler(m_ResizeHandler);
	}

	void Compositor::SetRenderer(Renderer& renderer)
	{
		m_pRenderer = &renderer;
	}

	bool Compositor::RenderFrame()
	{
		if (m_ShouldRecreateSwapchain)
		{
			RecreateSwapchain();
			m_ShouldRecreateSwapchain = false;
		}

		m_Dloader.vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame],
			VK_TRUE, UINT64_MAX);

		uint32_t imageIndex = 0;
		VkResult imageResult = m_Dloader.vkAcquireNextImageKHR(m_Device,
			m_Swapchain->Get(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame],
			VK_NULL_HANDLE, &imageIndex);

		if (imageResult != VK_SUCCESS)
		{
			m_ShouldRecreateSwapchain = true;
			return imageResult == VK_ERROR_OUT_OF_DATE_KHR ||
				imageResult == VK_SUBOPTIMAL_KHR;
		}

		RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

		m_Dloader.vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

		VkQueue queue = m_Context.GetQueue();

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

		if (m_Dloader.vkQueueSubmit(queue, 1, &submitInfo,
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

		if (m_Dloader.vkQueuePresentKHR(queue, &presentInfo)
			!= VK_SUCCESS)
			return false;

		m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;
		return true;
	}

	void Compositor::CheckPresentSupport()
	{
		VkBool32 presentSupport = false;
		m_Context.GetInstanceLoader().vkGetPhysicalDeviceSurfaceSupportKHR(
			m_Context.GetPhysicalDevice(), indices.graphicsFamilyIndex, 
			m_Surface.Get(), &presentSupport
		);

		if (!presentSupport)
			throw std::runtime_error("Device has no present queue family");
	}

	void Compositor::CreateSwapchain()
	{	
		m_Swapchain.emplace(
			m_Context,
			indices.graphicsFamilyIndex,
			m_SwapchainDetails,
			m_SurfaceFormat,
			m_Surface.Get(),
			m_Window.GetWidth(),
			m_Window.GetHeight(),
			true
		);

		m_SwapchainImages = m_Swapchain->GetImages();
		m_Swapchain->CreateImageViews(&m_SwapchainImageViews);
	}

	void Compositor::CreateFramebuffers()
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
			createInfo.renderPass = m_RenderPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = swapchainExtent.width;
			createInfo.height = swapchainExtent.height;
			createInfo.layers = 1;

			if (m_Dloader.vkCreateFramebuffer(m_Device, &createInfo, nullptr,
				&m_SwapchainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Framebuffer creation failed");
		}
	}

	void Compositor::CreateSyncObjects()
	{
		m_ImageAvailableSemaphores.resize(m_FramesInFlight);
		m_RenderFinishedSemaphores.resize(m_FramesInFlight);
		m_InFlightFences.resize(m_FramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < m_FramesInFlight; i++)
		{
			if (m_Dloader.vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
				&m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Semaphore creation failed");

			if (m_Dloader.vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr,
				&m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Semaphore creation failed");

			if (m_Dloader.vkCreateFence(m_Device, &fenceInfo, nullptr,
				&m_InFlightFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Fence creation failed");
		}
	}

	void Compositor::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_FramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Context.GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (m_Dloader.vkAllocateCommandBuffers(m_Device, &allocInfo,
			m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Command buffer allocation failed");
	}

	void Compositor::CreateRenderPass()
	{

	}

	void Compositor::ChooseSurfaceFormat()
	{
		const InstanceLoader& iloader = m_Context.GetInstanceLoader();
		VkPhysicalDevice physicalDevice = m_Context.GetPhysicalDevice();

		uint32_t formatCount;
		iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice, m_Surface.Get(),
			&formatCount, nullptr
		);

		if (formatCount == 0)
			throw std::runtime_error("Physical device has no surface formats");

		std::vector<VkSurfaceFormatKHR> formats;
		formats.resize(formatCount);

		iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice,
			m_Surface.Get(), &formatCount,
			formats.data()
		);

		for (const VkSurfaceFormatKHR& availableFormat : formats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
			{
				m_SurfaceFormat = availableFormat;
				return;
			}

		m_SurfaceFormat = formats[0];
	}

	void Compositor::QuerySwapchainSupport()
	{
		const InstanceLoader& iloader = m_Context.GetInstanceLoader();
		VkPhysicalDevice physicalDevice = m_Context.GetPhysicalDevice();
		VkSurfaceKHR vkSurface = m_Surface.Get();

		iloader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physicalDevice,
			vkSurface, &m_SwapchainDetails.capabilities
		);

		uint32_t formatCount;
		iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice, vkSurface,
			&formatCount, nullptr
		);

		if (formatCount != 0)
		{
			m_SwapchainDetails.formats.resize(formatCount);
			iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
				physicalDevice,
				vkSurface, &formatCount,
				m_SwapchainDetails.formats.data()
			);
		}

		uint32_t presentModeCount;
		iloader.vkGetPhysicalDeviceSurfacePresentModesKHR(
			physicalDevice,
			vkSurface, &presentModeCount,
			m_SwapchainDetails.presentModes.data()
		);

		if (presentModeCount != 0)
		{
			m_SwapchainDetails.presentModes.resize(presentModeCount);
			iloader.vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice,
				vkSurface, &presentModeCount,
				m_SwapchainDetails.presentModes.data()
			);
		}
	}

	void Compositor::RecordCommandBuffer(VkCommandBuffer commandBuffer,
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
				m_ClearColor.w };

			VkExtent2D swapchainExtent = m_Swapchain->GetExtent();

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_RenderPass;
			renderPassInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchainExtent;
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			m_Dloader.vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);

			m_pRenderer->SetViewportExtent(swapchainExtent);
			m_pRenderer->PopulateCommandBuffer(commandBuffer);

			m_Dloader.vkCmdEndRenderPass(commandBuffer);
		}
		if (m_Dloader.vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to end command buffer!");
	}

	bool Compositor::RecreateSwapchain()
	{
		if (m_Window.GetWidth() == 0 || m_Window.GetHeight() == 0)
			return true;

		DestroySwapchain();

		// The min and max window size fields in capabilities change on resize
		m_Context.GetInstanceLoader().vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			m_Context.GetPhysicalDevice(),
			m_Surface.Get(), &m_SwapchainDetails.capabilities
		);

		CreateSwapchain();
		CreateFramebuffers();

		return true;
	}

	void Compositor::DestroySwapchain()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
			m_Dloader.vkDestroyFramebuffer(m_Device, m_SwapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : m_SwapchainImageViews)
			m_Dloader.vkDestroyImageView(m_Device, imageView, nullptr);
	}
}