#pragma once

#include <Tether/Events/EventHandler.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleVulkanContext.hpp>

#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SimpleRenderer : public VulkanRenderer
	{
	public:
		SimpleRenderer(SimpleWindow& window, SimpleVulkanContext& context);
		~SimpleRenderer();
		TETHER_NO_COPY(SimpleRenderer);

		bool RenderFrame();
	private:
		void CreateSwapchain();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		bool RecreateSwapchain();
		void DestroySwapchain();

		SimpleWindow& m_Window;
		SimpleVulkanContext& m_Context;
		DeviceLoader& m_Dloader;
		
		class ResizeHandler : public Events::EventHandler
		{
		public:
			ResizeHandler(SimpleRenderer& renderer);

			void OnWindowResize(Events::WindowResizeEvent event) override;
		private:
			SimpleRenderer& m_Renderer;
		};
		ResizeHandler m_ResizeHandler;

		std::optional<Swapchain> m_Swapchain;

		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VkFramebuffer> m_SwapchainFramebuffers;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		bool m_ShouldRecreateSwapchain = false;
		
		uint32_t m_CurrentFrame = 0;
	};
}