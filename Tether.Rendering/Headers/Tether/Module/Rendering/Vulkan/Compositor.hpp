#pragma once

#include <Tether/Events/EventHandler.hpp>

#include <Tether/Module/Rendering/Compositor.hpp>

#include <Tether/Module/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

namespace Tether::Rendering::Vulkan
{
	class Renderer;
	class TETHER_EXPORT Compositor : public Rendering::Compositor
	{
		friend Renderer;
	public:
		Compositor(GraphicsContext& context, Window& window, 
			bool autoRecreateSwapchain = true);
		~Compositor();
		TETHER_NO_COPY(Compositor);

		void SetRenderer(Renderer& renderer);

		bool RenderFrame() override;

		bool RecreateSwapchain();
	protected:
		GraphicsContext& m_Context;
		VkRenderPass m_RenderPass = nullptr;
	private:
		void Init();

		void CheckPresentSupport();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();
		void CreateRenderPass();

		void ChooseSurfaceFormat();
		void QuerySwapchainSupport();
		
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		void DestroySwapchain();

		Renderer* m_pRenderer = nullptr;
		
		Window& m_Window;
		Surface m_Surface;

		uint32_t m_FramesInFlight = 2;
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;

		QueueFamilyIndices indices;
		VkSurfaceFormatKHR m_SurfaceFormat;

		SwapchainDetails m_SwapchainDetails;

		VkQueue m_PresentQueue = nullptr;
		
		class ResizeHandler : public Events::EventHandler
		{
		public:
			ResizeHandler(Compositor& renderer);

			void OnWindowResize(const Events::WindowResizeEvent& event) override;
		private:
			Compositor& m_Renderer;
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
		const bool m_AutoRecreateSwapchain = true;

		uint32_t m_CurrentFrame = 0;
	};
}
