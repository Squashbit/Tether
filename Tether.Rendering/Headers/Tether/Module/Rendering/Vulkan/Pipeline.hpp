#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/PipelineInfo.hpp>

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT PipelineLayout
	{
	public:
		PipelineLayout(Device* device, VkPipelineLayoutCreateInfo* createInfo);
		~PipelineLayout();
		TETHER_NO_COPY(PipelineLayout);

		VkPipelineLayout Get();
	private:
		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;

		VkPipelineLayout layout = nullptr;
	};

	class TETHER_EXPORT Pipeline
	{
	public:
		Pipeline(
			Device* device, VkRenderPass renderPass,
			VkExtent2D viewportExtent, uint32_t subpass,
			uint32_t* pVertexCode, size_t vertexCodeSize,
			uint32_t* pFragmentCode, size_t fragmentCodeSize,
			std::vector<VkVertexInputBindingDescription>& bindingDescs,
			std::vector<VkVertexInputAttributeDescription>& attribDescs,
			VkPipelineLayoutCreateInfo* pLayoutInfo = nullptr
		);
		~Pipeline();
		TETHER_NO_COPY(Pipeline);

		VkPipeline Get();
		VkPipelineLayout GetLayout();
	private:
		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;

		VkPipeline pipeline = nullptr;
		std::optional<PipelineLayout> layout;
	};
}
