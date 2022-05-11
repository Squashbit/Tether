#include <Tether/Application.hpp>
#include <Tether/NativeVulkan.hpp>

using namespace Tether;

void Application::InitVulkan()
{
	void* vulkanLibrary;
#if defined(_WIN32)
	vulkanLibrary = LoadLibrary("vulkan-1.dll");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	vulkanLibrary = LoadLibrary("libvulkan.so");
#else
	vulkanLibrary = LoadLibrary("libvulkan.so.1");
#endif

	if (!vulkanLibrary)
		return;

	vulkan = new Storage::VulkanNative();
	vulkan->handle = vulkanLibrary;
}

void Application::FreeVulkan()
{
	if (!vulkan)
		return;

	FreeLibrary(vulkan->handle);
}
