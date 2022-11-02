#ifndef _TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP
#define _TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP

namespace Tether::Rendering::Vulkan
{
	enum class ErrorCode
	{
		SUCCESS,
		UNKNOWN,
		// The GPU probably doesn't support the selected Vulkan version.
		INCOMPATIBLE_DRIVER,
		APP_INIT_FAILED, 
		VULKAN_NOT_INITIALIZED, 
		SURFACE_INIT_FAILED, 
		DEVICE_NOT_FOUND,
		DEVICE_INIT_FAILED,
		// Can be caused by many different things. 
		// No graphics queue family, failed to create image views, etc.
		SWAPCHAIN_INIT_FAILED,
		RENDERPASS_INIT_FAILED,
		SHADER_INIT_FAILED,
	};
}

#endif //_TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP