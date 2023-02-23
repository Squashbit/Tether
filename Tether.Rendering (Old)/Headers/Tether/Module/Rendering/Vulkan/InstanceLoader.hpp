#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/Defs.hpp>

#include <vulkan/vulkan.h>

#define TETHER_INSTANCE_FUNC_VAR(name) PFN_vk##name vk##name = nullptr;

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT InstanceLoader
	{
	public:
		void Load(VkInstance* pInstance, PFN_vkGetInstanceProcAddr getProcAddr);

		// different breed
		PFN_vkVoidFunction vkCreateXlibSurfaceKHR = nullptr;
		PFN_vkVoidFunction vkCreateWin32SurfaceKHR = nullptr;

		TETHER_INSTANCE_FUNC_VAR(CreateDebugUtilsMessengerEXT);
		TETHER_INSTANCE_FUNC_VAR(CreateDevice);
		TETHER_INSTANCE_FUNC_VAR(DestroyDebugUtilsMessengerEXT);
		TETHER_INSTANCE_FUNC_VAR(DestroyInstance);
		TETHER_INSTANCE_FUNC_VAR(DestroySurfaceKHR);
		TETHER_INSTANCE_FUNC_VAR(EnumeratePhysicalDevices);
		TETHER_INSTANCE_FUNC_VAR(EnumerateDeviceExtensionProperties);
		TETHER_INSTANCE_FUNC_VAR(GetDeviceProcAddr);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceFeatures);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceProperties);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceQueueFamilyProperties);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceSurfaceFormatsKHR);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceSurfacePresentModesKHR);
		TETHER_INSTANCE_FUNC_VAR(GetPhysicalDeviceSurfaceSupportKHR);
	};
}
