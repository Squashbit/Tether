// Useful for quickly creating a Render Context Native without having to initialize 
// all Vulkan objects manually. 
// See AdvancedNative to manually specify Vulkan objects.

#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/ErrorCode.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/ShaderModule.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>
#include <Tether/Module/Rendering/RenderContext.hpp>

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SimpleNative : public Renderer::IRenderContextNative
	{
	public:
		SimpleNative() = default;
		virtual ~SimpleNative() {}
		TETHER_NO_COPY(SimpleNative);

		ErrorCode Init(SimpleWindow* pWindow);
	private:
		bool RenderFrame();

		bool CreateDevice();
		bool CreateAllocator();
		bool CreateSwapchain();
		bool CreateRenderPass();
		bool CreateShaders();
		bool CreatePipeline();
		bool CreateFramebuffers();
		bool CreateSyncObjects();
		bool CreateCommandPool();
		bool CreateVertexBuffers();
		bool CreateCommandBuffer();

		bool PopulateCommandBuffers();
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);

		bool RecreateSwapchain();
		void DestroySwapchain();

		VkSurfaceFormatKHR ChooseSurfaceFormat(SwapchainDetails details);
		uint32_t FindImageCount(SwapchainDetails details);

		void OnDispose();

		VmaAllocator allocator;

		VertexBuffer square;

		SimpleWindow* pWindow = nullptr;

		InstanceLoader* iloader = nullptr;
		Instance* instance = nullptr;

		DeviceLoader* dloader = nullptr;
		Device device;

		Surface surface;
		Swapchain swapchain;

		Vulkan::QueueFamilyIndices queueIndices;
		VkPhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkRenderPass renderPass;

		Pipeline pipeline;

		Vulkan::ShaderModule vertexModule;
		Vulkan::ShaderModule fragmentModule;

		VkCommandPool commandPool;
		
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkFramebuffer> swapchainFramebuffers;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool shouldRecreateSwapchain = false;
		bool shouldRecreateCommandBuffers = false;

		uint32_t currentFrame = 0;
		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	};
}
