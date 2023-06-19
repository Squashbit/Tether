#include <Tether/Rendering/Vulkan/WindowRenderTargetVk.hpp>

#include <Tether.Rendering/Assets/CompiledShaders/solid.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/solid.frag.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/text.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/text.frag.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/textured.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/textured.frag.spv.h>

namespace Tether::Rendering::Vulkan
{
	WindowRenderTarget::ResizeHandler::ResizeHandler(WindowRenderTarget& renderTarget)
		:
		m_RenderTarget(renderTarget)
	{}

	void WindowRenderTarget::ResizeHandler::OnWindowResize(
		const Events::WindowResizeEvent& event)
	{
		if (m_RenderTarget.m_AutoRecreateSwapchain)
			m_RenderTarget.RecreateSwapchain();
	}

	WindowRenderTarget::WindowRenderTarget(GraphicsContext& context,
		Window& window)
		:
		m_Context(context),
		m_Window(window),
		m_Device(context.GetDevice()),
		m_Iloader(context.GetInstanceLoader()),
		m_Dloader(context.GetDeviceLoader()),
		m_Surface(context, window),
		m_SolidPipeline(CreateSolidPipeline()),
		m_TexturedPipeline(CreateTexturedPipeline()),
		m_TextPipeline(CreateTextPipeline()),
		m_Renderer(context, m_SolidPipeline, m_TexturedPipeline, m_TextPipeline),
		m_ResizeHandler(*this),
		m_FramesInFlight(context.GetFramesInFlight())
	{
		if (!IsPresentationSupported())
			throw std::runtime_error("Presentation not supported");

		QuerySwapchainSupport();

		ChooseSurfaceFormat();
		CreateRenderPass();

		CreateSwapchain();
		CreateFramebuffers();
		CreateSyncObjects();
		CreateCommandBuffers();

		m_Window.AddEventHandler(m_ResizeHandler,
			Events::EventType::WINDOW_RESIZE);
	}

	WindowRenderTarget::~WindowRenderTarget()
	{
		m_Dloader.vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}

	void WindowRenderTarget::StartRender()
	{
		if (m_IsRendering)
			throw std::runtime_error("StartRender called while still rendering");
		m_IsRendering = true;

		if (m_ShouldRecreateSwapchain)
		{
			RecreateSwapchain();
			m_ShouldRecreateSwapchain = false;
		}

		m_Dloader.vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame],
			VK_TRUE, UINT64_MAX);

		m_Renderer.StartNewFrame(m_CurrentFrame, m_CommandBuffers[m_CurrentFrame],
			m_Swapchain->GetExtent());
	}

	bool WindowRenderTarget::EndRender()
	{
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
		presentInfo.pImageIndices = &m_ImageIndex;

		if (m_Dloader.vkQueuePresentKHR(queue, &presentInfo)
			!= VK_SUCCESS)
			return false;

		m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;

		m_IsRendering = false;
		return true;
	}

	bool WindowRenderTarget::RecreateSwapchain()
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

	Renderer& WindowRenderTarget::GetRenderer()
	{
		return m_Renderer;
	}

	Pipeline WindowRenderTarget::CreateSolidPipeline()
	{
		using namespace Assets;

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(VulkanRenderer::RectanglePushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			| VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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

		return Pipeline(
			m_Context, m_RenderPass, {}, 0,
			(uint32_t*)VulkanShaders::_binary_solid_vert_spv,
			sizeof(VulkanShaders::_binary_solid_vert_spv),
			(uint32_t*)VulkanShaders::_binary_solid_frag_spv,
			sizeof(VulkanShaders::_binary_solid_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	Pipeline WindowRenderTarget::CreateTexturedPipeline()
	{
		using namespace Assets;

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Device,
			&layoutInfo, nullptr, &m_TexturedPipelineSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout");

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(VulkanRenderer::ImagePushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_TexturedPipelineSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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

		return Pipeline(
			m_Context, m_RenderPass, {}, 0,
			(uint32_t*)VulkanShaders::_binary_textured_vert_spv,
			sizeof(VulkanShaders::_binary_textured_vert_spv),
			(uint32_t*)VulkanShaders::_binary_textured_frag_spv,
			sizeof(VulkanShaders::_binary_textured_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	Pipeline WindowRenderTarget::CreateTextPipeline()
	{
		using namespace Assets;

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr,
			&m_TextPipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout");

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(VulkanRenderer::TextPushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			| VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_TextPipelineLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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

		return Pipeline(
			m_Context, m_RenderPass, {}, 0,
			(uint32_t*)VulkanShaders::_binary_text_vert_spv,
			sizeof(VulkanShaders::_binary_text_vert_spv),
			(uint32_t*)VulkanShaders::_binary_text_frag_spv,
			sizeof(VulkanShaders::_binary_text_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void WindowRenderTarget::QuerySwapchainSupport()
	{
		VkPhysicalDevice physicalDevice = m_Context.GetPhysicalDevice();
		VkSurfaceKHR vkSurface = m_Surface.Get();

		m_Iloader.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physicalDevice,
			vkSurface, &m_SwapchainDetails.capabilities
		);

		uint32_t formatCount;
		m_Iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice, vkSurface,
			&formatCount, nullptr
		);

		if (formatCount != 0)
		{
			m_SwapchainDetails.formats.resize(formatCount);
			m_Iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
				physicalDevice,
				vkSurface, &formatCount,
				m_SwapchainDetails.formats.data()
			);
		}

		uint32_t presentModeCount;
		m_Iloader.vkGetPhysicalDeviceSurfacePresentModesKHR(
			physicalDevice,
			vkSurface, &presentModeCount,
			m_SwapchainDetails.presentModes.data()
		);

		if (presentModeCount != 0)
		{
			m_SwapchainDetails.presentModes.resize(presentModeCount);
			m_Iloader.vkGetPhysicalDeviceSurfacePresentModesKHR(
				physicalDevice,
				vkSurface, &presentModeCount,
				m_SwapchainDetails.presentModes.data()
			);
		}
	}

	bool WindowRenderTarget::IsPresentationSupported()
	{
		VkBool32 presentSupport = false;
		m_Iloader.vkGetPhysicalDeviceSurfaceSupportKHR(
			m_Context.GetPhysicalDevice(), indices.graphicsFamilyIndex,
			m_Surface.Get(), &presentSupport
		);

		if (!presentSupport)
			throw std::runtime_error("Device has no present queue family");
	}

	void WindowRenderTarget::ChooseSurfaceFormat()
	{
		VkPhysicalDevice physicalDevice = m_Context.GetPhysicalDevice();

		uint32_t formatCount;
		m_Iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
			physicalDevice, m_Surface.Get(),
			&formatCount, nullptr
		);

		if (formatCount == 0)
			throw std::runtime_error("Physical device has no surface formats");

		std::vector<VkSurfaceFormatKHR> formats;
		formats.resize(formatCount);

		m_Iloader.vkGetPhysicalDeviceSurfaceFormatsKHR(
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

	void WindowRenderTarget::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_SurfaceFormat.format;
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

		if (m_Dloader.vkCreateRenderPass(m_Device, &desc,
			nullptr, &m_RenderPass) != VK_SUCCESS)
			throw std::runtime_error("Render pass creation failed");
	}

	void WindowRenderTarget::CreateSwapchain()
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

	void WindowRenderTarget::CreateFramebuffers()
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

	void WindowRenderTarget::CreateSyncObjects()
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

	void WindowRenderTarget::CreateCommandBuffers()
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

	void WindowRenderTarget::DestroySwapchain()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
			m_Dloader.vkDestroyFramebuffer(m_Device, m_SwapchainFramebuffers[i],
				nullptr);

		for (VkImageView imageView : m_SwapchainImageViews)
			m_Dloader.vkDestroyImageView(m_Device, imageView, nullptr);
	}
}