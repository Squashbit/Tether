#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Allocator.hpp>
#include <Tether/Module/Rendering/Vulkan/RenderPass.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

#include <optional>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanRenderer : public Rendering::Renderer
	{
	public:
		VulkanRenderer(SimpleWindow* pWindow);
		~VulkanRenderer();
		TETHER_NO_COPY(VulkanRenderer);

		bool RenderFrame();
	private:
		void WaitForCommandBuffers();

		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

		void OnCreateObject(Scope<Objects::Rectangle>& object) override;
		void OnCreateObject(Scope<Objects::Image>& object) override;
		void OnCreateObject(Scope<Objects::Text>& object) override;

		void OnCreateResource(Scope<Resources::BufferedImage>& image,
			const Resources::BufferedImageInfo& info) override;
		void OnCreateResource(Scope<Resources::Font>& font,
			const std::string& fontPath) override;

		void CreateSwapchain();
		void CreateSolidPipeline();
		void CreateTexturedPipeline();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void CreateVertexBuffers();
		void CreateSampler();

		VkSurfaceFormatKHR ChooseSurfaceFormat();
		SwapchainDetails QuerySwapchainSupport();

		bool PopulateCommandBuffers(uint32_t imageIndex);
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void AddObjectsToCommandBuffer(VkCommandBuffer commandBuffer);

		bool RecreateSwapchain();
		void DestroySwapchain();
		
		SimpleWindow* pWindow = nullptr;
		Instance* instance = nullptr;
		InstanceLoader* iloader = nullptr;
		Surface surface;
		Device device;
		DeviceLoader* dloader = nullptr;
		Allocator allocator;
		SwapchainDetails swapchainDetails;
		VkSurfaceFormatKHR surfaceFormat;
		RenderPass renderPass;

		VkCommandPool commandPool;
		VkSampler sampler;

		std::optional<Pipeline> solidPipeline;
		
		std::optional<Pipeline> texturedPipeline;
		VkDescriptorSetLayout texturedPipelineSetLayout;

		std::optional<VertexBuffer> square;
		std::optional<Swapchain> swapchain;

		Vulkan::QueueFamilyIndices queueIndices;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkFramebuffer> swapchainFramebuffers;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool shouldRecreateSwapchain = false;
		
		uint32_t currentFrame = 0;
	};
}
