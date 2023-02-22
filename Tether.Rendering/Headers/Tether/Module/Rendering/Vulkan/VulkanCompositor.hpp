#pragma once

#include <Tether/Events/EventHandler.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanWindow.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleVulkanContext.hpp>

#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanCompositor : public VulkanRenderer
	{
	public:
		VulkanCompositor(VulkanRenderer& renderer, VulkanWindowContext& context);
		~VulkanCompositor();
		TETHER_NO_COPY(VulkanCompositor);

		bool RenderFrame() override;
	private:
		void Init();

		void CreatePresentQueue();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();

		void QuerySwapchainSupport();
		void ChooseSurfaceFormat();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		bool RecreateSwapchain();
		void DestroySwapchain();

		VulkanRenderer& m_Renderer;
		Window& m_Window;
		SimpleVulkanContext& m_Context;
		Surface m_Surface;
		DeviceLoader& m_Dloader;

		uint32_t m_PresentFamilyIndex = 0;
		
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

		Math::Vector3f m_ClearColor;
		
		uint32_t m_CurrentFrame = 0;
	};
}
