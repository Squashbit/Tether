#pragma once

#include <Tether/Window.hpp>
#include <Tether/Rendering/RenderTarget.hpp>
#include <Tether/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Rendering/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>
#include <Tether/Rendering/Vulkan/Swapchain.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT WindowRenderTarget : public RenderTarget
	{
	public:
		WindowRenderTarget(GraphicsContext& context, Window& window);
		~WindowRenderTarget();
		TETHER_NO_COPY(WindowRenderTarget);

		void StartRender() override;
		bool EndRender() override;

		bool RecreateSwapchain();

		Renderer& GetRenderer() override;
	private:
		Pipeline CreateSolidPipeline();
		Pipeline CreateTexturedPipeline();
		Pipeline CreateTextPipeline();

		void QuerySwapchainSupport();
		bool IsPresentationSupported();

		void ChooseSurfaceFormat();
		void CreateRenderPass();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandBuffers();

		void DestroySwapchain();

		GraphicsContext& m_Context;
		Window& m_Window;

		VkDevice m_Device = nullptr;
		const InstanceLoader& m_Iloader;
		const DeviceLoader& m_Dloader;

		Surface m_Surface;
		VkSurfaceFormatKHR m_SurfaceFormat;
		QueueFamilyIndices indices;
		VkRenderPass m_RenderPass = nullptr;

		SwapchainDetails m_SwapchainDetails;

		Pipeline m_SolidPipeline;
		Pipeline m_TexturedPipeline;
		Pipeline m_TextPipeline;

		VkDescriptorSetLayout m_TexturedPipelineSetLayout;
		VkDescriptorSetLayout m_TextPipelineLayout;
		
		VulkanRenderer m_Renderer;

		VkQueue m_PresentQueue = nullptr;

		class ResizeHandler : public Events::EventHandler
		{
		public:
			ResizeHandler(WindowRenderTarget& renderTarget);

			void OnWindowResize(const Events::WindowResizeEvent& event) override;
		private:
			WindowRenderTarget& m_RenderTarget;
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

		bool m_IsRendering = false;

		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrame = 0;
		const uint32_t m_FramesInFlight;
	};
}