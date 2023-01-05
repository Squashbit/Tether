#include <Tether/Application.hpp>
#include <Tether/Common/LibraryLoader.hpp>
#include <Tether/Module/Rendering/RenderingModule.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether;
using namespace Tether::Rendering;

bool RenderingModule::LoadVulkan()
{
	if (vulkan)
		return true;

	void* vulkanLibrary;
#if defined(_WIN32)
	vulkanLibrary = LibraryLoader::LoadLibrary("vulkan-1.dll");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
	vulkanLibrary = LibraryLoader::LoadLibrary("libvulkan.so");
#else
	vulkanLibrary = LibraryLoader::LoadLibrary("libvulkan.so.1");
#endif

	if (!vulkanLibrary)
		return false;

	vulkan = new(std::nothrow) Vulkan::VulkanNative();
	if (!vulkan)
	{
		LibraryLoader::FreeLibrary(vulkanLibrary);
		return false;
	} vulkan->handle = vulkanLibrary;

	vulkan->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
		LibraryLoader::LoadFunction(
		vulkan->handle, "vkGetInstanceProcAddr");

	TETHER_INSTANCE_FUNC_NULL(CreateInstance);
	TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceExtensionProperties);
	TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceLayerProperties);
	
	return true;
}

void RenderingModule::InitVulkan(bool validationlayers)
{
	if (!LoadVulkan())
		throw RendererException("Vulkan library not found");

	vulkan->instance.emplace("TetherApp", "Tether", validationlayers);
	vulkan->initialized = true;
}

Rendering::Vulkan::VulkanNative* RenderingModule::GetVulkanNative()
{
	return vulkan;
}

bool RenderingModule::IsVulkanLoaded()
{
	return vulkan;
}

bool RenderingModule::IsVulkanInitialized()
{
	if (!vulkan)
		return false;

	return vulkan->initialized;
}

void RenderingModule::DisposeVulkan()
{
	if (!vulkan)
		return;

	if (vulkan->initialized)
		vulkan->instance.reset();

	LibraryLoader::FreeLibrary(vulkan->handle);

	delete vulkan;
	vulkan = nullptr;
}
