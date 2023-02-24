#pragma once

#include <Tether/Events/EventHandler.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Compositor.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanWindow.hpp>

#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanCompositor : public Compositor
	{
	public:
		VulkanCompositor(VulkanRenderer& renderer, VulkanWindow& window);
		~VulkanCompositor();
		TETHER_NO_COPY(VulkanCompositor);

		bool RenderFrame() override;
	private:
		void Init();

		void CheckPresentSupport();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();

		void QuerySwapchainSupport();
		
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		bool RecreateSwapchain();
		void DestroySwapchain();

		VulkanRenderer& m_Renderer;
		VulkanWindow& m_VulkanWindow;
		Window& m_Window;
		VulkanContext& m_Context;

		DeviceLoader& m_Dloader;

		SwapchainDetails m_SwapchainDetails;

		VkQueue m_PresentQueue = nullptr;
		
		class ResizeHandler : public Events::EventHandler
		{
		public:
			ResizeHandler(VulkanCompositor& renderer);

			void OnWindowResize(Events::WindowResizeEvent event) override;
		private:
			VulkanCompositor& m_Renderer;
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
