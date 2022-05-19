#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/NativeVulkan.hpp>

using namespace Tether;

#define LOAD_VULKAN_FUNCTION(nativeName, type, name) \
	vulkan->nativeName = (type)LoadFunction(vulkan->handle, name); \
	if (!vulkan->nativeName) \
	{ \
		FreeLibrary(vulkan->handle); \
		delete vulkan; \
		return false; \
	}

bool Application::LoadVulkan()
{
	if (vulkan)
		return false;

	void* vulkanLibrary;
#if defined(_WIN32)
	vulkanLibrary = LoadLibrary("vulkan-1.dll");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	vulkanLibrary = LoadLibrary("libvulkan.so");
#else
	vulkanLibrary = LoadLibrary("libvulkan.so.1");
#endif

	if (!vulkanLibrary)
		return false;

	vulkan = new(std::nothrow) Storage::VulkanNative();
	if (!vulkan)
	{
		FreeLibrary(vulkanLibrary);
		return false;
	} vulkan->handle = vulkanLibrary;

	LOAD_VULKAN_FUNCTION(
		GetInstanceProcAddr, 
		PFN_vkGetInstanceProcAddr,
		"vkGetInstanceProcAddr"
	);

	vulkan->CreateInstance = 
		(PFN_vkCreateInstance)vkGetInstanceProcAddr(NULL, "vkCreateInstance");
	vulkan->EnumInstanceExtProps =
		(PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(NULL, 
			"vkEnumerateInstanceExtensionProperties");

	return true;
}

bool Application::InitVulkan()
{
	if (!LoadVulkan())
		return false;

	// Init stuff

	vulkan->initialized = true;
	return true;
}

Storage::VulkanNative* Application::GetVulkanNative()
{
	return vulkan;
}

void Application::DisposeVulkan()
{
	if (!vulkan)
		return;

	// Dispose loaded stuff
	FreeLibrary(vulkan->handle);

	// Then dispose Vulkan itself
	// ...when there's actually something to dispose
}
