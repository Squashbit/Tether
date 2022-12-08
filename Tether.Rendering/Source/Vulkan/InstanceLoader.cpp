#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether::Rendering::Vulkan;

void InstanceLoader::Load(VkInstance* pInstance)
{
#ifdef __linux__
	vkCreateXlibSurfaceKHR = vkGetInstanceProcAddr(
		*pInstance, "vkCreateXlibSurfaceKHR");
#elif _WIN32
	vkCreateWin32SurfaceKHR = vkGetInstanceProcAddr(
		*pInstance, "vkCreateWin32SurfaceKHR");
#endif
	
	TETHER_INSTANCE_FUNC(CreateDebugUtilsMessengerEXT);
	TETHER_INSTANCE_FUNC(CreateDevice);
	TETHER_INSTANCE_FUNC(DestroyDebugUtilsMessengerEXT);
	TETHER_INSTANCE_FUNC(DestroyInstance);
	TETHER_INSTANCE_FUNC(DestroySurfaceKHR);
	TETHER_INSTANCE_FUNC(EnumeratePhysicalDevices);
	TETHER_INSTANCE_FUNC(EnumerateDeviceExtensionProperties);
	TETHER_INSTANCE_FUNC(GetDeviceProcAddr);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceFeatures);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceProperties);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceQueueFamilyProperties);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceSurfaceFormatsKHR);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceSurfacePresentModesKHR);
	TETHER_INSTANCE_FUNC(GetPhysicalDeviceSurfaceSupportKHR);
}
