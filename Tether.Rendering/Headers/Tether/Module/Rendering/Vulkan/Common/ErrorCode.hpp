#ifndef _TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP
#define _TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP

namespace Tether::Rendering::Vulkan
{
	enum class ErrorCode
	{
		// Success!
		SUCCESS,
		// The vulkan instance in the Application failed to initialize
		APP_INIT_FAILED, 
		// Failed to create the surface
		SURFACE_INIT_FAILED, 
		// No suitable GPU was found
		DEVICE_NOT_FOUND,
		// Failed to initialize the device.
		DEVICE_INIT_FAILED,
		// Can be caused by many different things. 
		// No graphics queue family, failed to create image views, etc.
		SWAPCHAIN_INIT_FAILED,
		// Failed to initialize the RenderPass.
		RENDERPASS_INIT_FAILED,
	};
}

#endif //_TETHER_RENDERER_VULKAN_COMMON_ERRORCODE_HPP