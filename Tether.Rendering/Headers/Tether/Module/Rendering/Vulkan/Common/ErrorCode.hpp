#pragma once

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
		PIPELINE_INIT_FAILED,
		FRAMEBUFFER_INIT_FAILED,
		COMMANDPOOL_INIT_FAILED,
		COMMANDBUFFER_INIT_FAILED,
		SYNC_OBJECT_INIT_FAILED,
	};
}
