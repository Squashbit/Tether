#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ShaderModule
	{
	public:
		ShaderModule(
			VulkanContext& context,
			VkShaderStageFlagBits stage,
			uint32_t* pCode, size_t codeSize
		);
		~ShaderModule();
		TETHER_NO_COPY(ShaderModule);

		VkShaderModule Get();
		VkPipelineShaderStageCreateInfo GetStageInfo();
	private:
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;

		VkShaderModule shader = nullptr;
		VkPipelineShaderStageCreateInfo stageInfo{};
	};
}