#ifndef _TETHER_DEVICELOADER_HPP
#define _TETHER_DEVICELOADER_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT DeviceLoader
	{
	public:
		DeviceLoader() = default;
		
		void Load(InstanceLoader* pInstLoader, VkDevice* pDevice);

		TETHER_DEVICE_FUNC_VAR(CreateImageView);
		TETHER_DEVICE_FUNC_VAR(CreateRenderPass);
		TETHER_DEVICE_FUNC_VAR(CreateShaderModule);
		TETHER_DEVICE_FUNC_VAR(CreateSwapchainKHR);
		TETHER_DEVICE_FUNC_VAR(DestroyDevice);
		TETHER_DEVICE_FUNC_VAR(DestroyRenderPass);
		TETHER_DEVICE_FUNC_VAR(DestroyShaderModule);
		TETHER_DEVICE_FUNC_VAR(DestroySwapchainKHR);
		TETHER_DEVICE_FUNC_VAR(DeviceWaitIdle);
		TETHER_DEVICE_FUNC_VAR(GetDeviceQueue);
		TETHER_DEVICE_FUNC_VAR(GetSwapchainImagesKHR);
	};
}

#endif //_TETHER_DEVICELOADER_HPP