#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/AllocatorManager.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <optional>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanRenderer : public Rendering::Renderer
	{
	public:
		VulkanRenderer(const VulkanContext& context);
		~VulkanRenderer();
		TETHER_NO_COPY(VulkanRenderer);

		void SetSwapchainExtent(VkExtent2D swapchainExtent);
		void PopulateCommandBuffer(VkCommandBuffer commandBuffer);
	private:
		void CreateAllocator();
		void CreateSolidPipeline();
		void CreateTexturedPipeline();
		void CreateTextPipeline();

		VulkanContext m_Context;
		DeviceLoader& m_Dloader;

		std::optional<AllocatorManager> m_Allocator;

		std::optional<Pipeline> solidPipeline;
		
		std::optional<Pipeline> texturedPipeline;
		VkDescriptorSetLayout texturedPipelineSetLayout;

		std::optional<Pipeline> textPipeline;
		VkDescriptorSetLayout textPipelineLayout;

		Vulkan::QueueFamilyIndices queueIndices;
		VkQueue graphicsQueue;
		
		VkExtent2D m_SwapchainExtent;

		uint32_t currentFrame = 0;
	};
}
