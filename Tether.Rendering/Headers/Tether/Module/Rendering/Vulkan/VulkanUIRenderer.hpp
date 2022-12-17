#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/UIRenderer.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/ErrorCode.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanUIRenderer : public Rendering::UIRenderer
	{
	public:
		VulkanUIRenderer() = default;
		TETHER_DISPOSE_ON_DESTROY(VulkanUIRenderer);
		VulkanUIRenderer(SimpleWindow* pWindow);
		
		bool RenderFrame();

		VertexBuffer* GetRectangleBuffer();
		DeviceLoader* GetDeviceLoader();
	private:
		bool OnObjectCreate(HashedString& typeName, Objects::Object* pObject);
		void OnObjectAdd(Objects::Object* pObject);
		void OnObjectRemove(Objects::Object* pObject);

		void CreateDevice();
		void CreateAllocator();
		void CreateSwapchain();
		void CreateRenderPass();
		void CreatePipeline();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void CreateCommandPool();
		void CreateVertexBuffers();
		void CreateCommandBuffer();

		bool PopulateCommandBuffers();
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
		void AddObjectsToCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);

		bool RecreateSwapchain();
		void DestroySwapchain();

		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool PickDevice();

		VkSurfaceFormatKHR ChooseSurfaceFormat(SwapchainDetails details);
		uint32_t FindImageCount(SwapchainDetails details);

		void OnRendererDispose();

		VmaAllocator allocator;

		SimpleWindow* pWindow = nullptr;
		InstanceLoader* iloader = nullptr;
		Instance* instance = nullptr;
		DeviceLoader* dloader = nullptr;

		Scope<Surface> surface;
		Scope<Device> device;
		Scope<Swapchain> swapchain;
		Scope<Pipeline> pipeline;
		Scope<VertexBuffer> square;
		
		Vulkan::QueueFamilyIndices queueIndices;
		VkPhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkRenderPass renderPass;

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
