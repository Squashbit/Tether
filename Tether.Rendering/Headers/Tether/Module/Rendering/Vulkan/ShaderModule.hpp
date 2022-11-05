#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/ShaderType.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ShaderModule : public IDisposable
	{
	public:
		ShaderModule() = default;
		TETHER_DISPOSE_ON_DESTRUCT(ShaderModule);
		TETHER_NO_COPY(ShaderModule);

		bool CreateFromSpirV(Device* pDevice, ShaderType shaderType, 
			uint32_t* pBytecode, size_t bytecodeSize);

		VkShaderModule Get();
	private:
		void OnDispose();

		ShaderType shaderType;
		VkShaderModule vkShaderModule;

		DeviceLoader* dloader;
		Device* pDevice = nullptr;
	};
}
