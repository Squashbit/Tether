#include <Tether/Module/Rendering/Vulkan/ShaderModule.hpp>

using namespace Tether::Rendering::Vulkan;

bool ShaderModule::CreateFromSpirV(Device* pDevice, ShaderType shaderType,
	uint32_t* pBytecode, size_t bytecodeSize)
{
	if (initialized)
		return false;

	this->pDevice = pDevice;
	this->dloader = pDevice->GetLoader();
	this->shaderType = shaderType;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = pBytecode;
	createInfo.codeSize = bytecodeSize;

	if (dloader->vkCreateShaderModule(pDevice->Get(), &createInfo, nullptr,
		&vkShaderModule) != VK_SUCCESS)
		return false;

	initialized = true;
	return true;
}

VkShaderModule ShaderModule::Get()
{
	return vkShaderModule;
}

void ShaderModule::OnDispose()
{
	dloader->vkDestroyShaderModule(pDevice->Get(), vkShaderModule, nullptr);
}