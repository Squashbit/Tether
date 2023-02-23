#include <Tether/Module/Rendering/Vulkan/ShaderModule.hpp>
#include <stdexcept>

using namespace Tether::Rendering::Vulkan;

ShaderModule::ShaderModule(
	Context& context,
	VkShaderStageFlagBits stage,
	uint32_t* pCode, size_t codeSize
)
	:
	m_Device(context.device),
	m_Dloader(context.deviceLoader)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = pCode;
	createInfo.codeSize = codeSize;

	if (m_Dloader.vkCreateShaderModule(m_Device, &createInfo, nullptr, &shader)
		!= VK_SUCCESS)
		throw std::runtime_error("Shader creation failed");

	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = stage;
	stageInfo.module = shader;
	stageInfo.pName = "main";
}

ShaderModule::~ShaderModule()
{
	m_Dloader.vkDestroyShaderModule(m_Device, shader, nullptr);
}

VkShaderModule ShaderModule::Get()
{
	return shader;
}

VkPipelineShaderStageCreateInfo ShaderModule::GetStageInfo()
{
	return stageInfo;
}
