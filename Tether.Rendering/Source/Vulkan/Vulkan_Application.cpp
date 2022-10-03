#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether;

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

	vulkan->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)LoadFunction(
		vulkan->handle, "vkGetInstanceProcAddr");

	TETHER_INSTANCE_FUNC_NULL(CreateInstance);
	TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceExtensionProperties);
	TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceLayerProperties);
	
	return true;
}

bool Application::InitVulkan(bool validationlayers)
{
	if (!LoadVulkan())
		return false;

	if (!vulkan->instance.Init("TetherApp", "Tether",
		validationlayers))
		return false;

	vulkan->initialized = true;
	return true;
}

Storage::VulkanNative* Application::GetVulkanNative()
{
	return vulkan;
}

bool Application::IsVulkanLoaded()
{
	return vulkan;
}

bool Application::IsVulkanInitialized()
{
	if (!vulkan)
		return false;

	return vulkan->initialized;
}

void Application::DisposeVulkan()
{
	if (!vulkan)
		return;

	// Dispose loaded stuff
	FreeLibrary(vulkan->handle);

	if (vulkan->initialized)
	{
		// Then dispose the instance
		vulkan->instance.Dispose();
	}

	delete vulkan;
	vulkan = nullptr;
}
