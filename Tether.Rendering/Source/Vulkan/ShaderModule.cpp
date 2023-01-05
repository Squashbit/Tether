#include <Tether/Module/Rendering/Vulkan/ShaderModule.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

using namespace Tether::Rendering::Vulkan;

ShaderModule::ShaderModule(
	Device* pDevice,
	VkShaderStageFlagBits stage,
	uint32_t* pCode, size_t codeSize
)
{
	this->pDevice = pDevice;
	this->dloader = pDevice->GetLoader();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = pCode;
	createInfo.codeSize = codeSize;

	if (dloader->vkCreateShaderModule(pDevice->Get(), &createInfo, nullptr, &shader)
		!= VK_SUCCESS)
		throw RendererException("Shader creation failed");

	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = stage;
	stageInfo.module = shader;
	stageInfo.pName = "main";
}

ShaderModule::~ShaderModule()
{
	dloader->vkDestroyShaderModule(pDevice->Get(), shader, nullptr);
}

VkShaderModule ShaderModule::Get()
{
	return shader;
}

VkPipelineShaderStageCreateInfo ShaderModule::GetStageInfo()
{
	return stageInfo;
}
