#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ShaderModule
	{
	public:
		ShaderModule(
			GraphicsContext& context,
			VkShaderStageFlagBits stage,
			uint32_t* pCode, size_t codeSize
		);
		~ShaderModule();
		TETHER_NO_COPY(ShaderModule);

		VkShaderModule Get();
		VkPipelineShaderStageCreateInfo GetStageInfo();
	private:
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;

		VkShaderModule shader = nullptr;
		VkPipelineShaderStageCreateInfo stageInfo{};
	};
}