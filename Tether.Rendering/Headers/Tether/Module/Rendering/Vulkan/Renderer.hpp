#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Pipeline.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>
#include <Tether/Module/Rendering/Vulkan/Compositor.hpp>

#include <Tether/Module/Rendering/Vulkan/GraphicsContext.hpp>

#include <optional>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Renderer
	{
	public:
		Renderer(GraphicsContext& context, VkRenderPass renderPass);
		Renderer(Compositor& compositor);
		~Renderer();
		TETHER_NO_COPY(Renderer);

		void SetViewportExtent(VkExtent2D viewport);

		using ObjectsIter = std::vector<Objects::Object*>::iterator;
		void PopulateCommandBuffer(VkCommandBuffer commandBuffer,
			ObjectsIter startIter, ObjectsIter endIter);
	private:
		void CreatePipelines(VkRenderPass renderPass);

		void CreateSolidPipeline(VkRenderPass renderPass);
		void CreateTexturedPipeline(VkRenderPass renderPass);
		void CreateTextPipeline(VkRenderPass renderPass);

		GraphicsContext& m_Context;

		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;

		std::optional<Pipeline> solidPipeline;
		std::optional<Pipeline> texturedPipeline;
		std::optional<Pipeline> textPipeline;

		VkDescriptorSetLayout texturedPipelineSetLayout;
		VkDescriptorSetLayout textPipelineLayout;

		VkExtent2D m_ViewportExtent;

		uint32_t currentFrame = 0;
	};
}
