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
		
		bool RenderFrame();

		Scope<BufferedImage> CreateImage(const BufferedImageInfo& info) override;

		void WaitForCommandBuffers();

		Pipeline& GetSolidPipeline();
		VkDescriptorSetLayout GetSolidPipelineSetLayout();

		Pipeline& GetTexturedPipeline();

		uint32_t GetSwapchainImageCount();
		Device* GetDevice();
		VertexBuffer* GetRectangleBuffer();
		VmaAllocator GetAllocator();

		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	private:
		void OnCreateObject(Scope<Objects::Rectangle>& object) override;
		void OnCreateObject(Scope<Objects::Image>& object) override;

		void OnObjectAdd(Objects::Object* pObject) override;
		void OnObjectRemove(Objects::Object* pObject) override;

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

		VkCommandPool commandPool;
		VkSampler sampler;

		std::optional<Pipeline> solidPipeline;
		VkDescriptorSetLayout solidPipelineSetLayout;

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
		bool shouldRecreateCommandBuffers = false;

		uint32_t currentFrame = 0;
	};
}
