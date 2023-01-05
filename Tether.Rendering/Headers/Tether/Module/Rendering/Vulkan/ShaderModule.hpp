#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ShaderModule
	{
	public:
		ShaderModule(
			Device* pDevice,
			VkShaderStageFlagBits stage,
			uint32_t* pCode, size_t codeSize
		);
		~ShaderModule();
		TETHER_NO_COPY(ShaderModule);

		VkShaderModule Get();
		VkPipelineShaderStageCreateInfo GetStageInfo();
	private:
		Device* pDevice = nullptr;
		DeviceLoader* dloader = nullptr;

		VkShaderModule shader = nullptr;
		VkPipelineShaderStageCreateInfo stageInfo{};
	};
}