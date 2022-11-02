#ifndef _TETHER_RENDERING_VULKAN_SHADERMODULE_HPP
#define _TETHER_RENDERING_VULKAN_SHADERMODULE_HPP

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
	private:
		void OnDispose();

		ShaderType shaderType;
		VkShaderModule vkShaderModule;

		DeviceLoader* dloader;
		Device* pDevice = nullptr;
	};
}

#endif //_TETHER_RENDERING_VULKAN_SHADERMODULE_HPP