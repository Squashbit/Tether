#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT PipelineLayout
	{
	public:
		PipelineLayout(VulkanContext& context, 
			VkPipelineLayoutCreateInfo* createInfo);
		~PipelineLayout();
		TETHER_NO_COPY(PipelineLayout);

		VkPipelineLayout Get();
	private:
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;

		VkPipelineLayout layout = nullptr;
	};

	class TETHER_EXPORT Pipeline
	{
	public:
		Pipeline(
			VulkanContext& context,
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
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;

		VkPipeline pipeline = nullptr;
		std::optional<PipelineLayout> layout;
	};
}
