#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/UIRenderer.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/ErrorCode.hpp>
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
	class TETHER_EXPORT VulkanUIRenderer : public Rendering::UIRenderer
	{
	public:
		VulkanUIRenderer(SimpleWindow* pWindow);
		~VulkanUIRenderer();
		
		bool RenderFrame();

		VertexBuffer* GetRectangleBuffer();
		DeviceLoader* GetDeviceLoader();
	private:
		Scope<Objects::ObjectRenderer> OnObjectCreateRenderer(
			HashedString& typeName, 
			Objects::Object* pObject
		) override;

		void OnObjectAdd(Objects::Object* pObject) override;
		void OnObjectRemove(Objects::Object* pObject) override;

		VkSurfaceFormatKHR ChooseSurfaceFormat();
		SwapchainDetails QuerySwapchainSupport();

		void CreateSwapchain();
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

		Vulkan::QueueFamilyIndices queueIndices;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		std::optional<Pipeline> pipeline;
		std::optional<VertexBuffer> square;

		std::optional<Swapchain> swapchain;
		
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
