#include <Tether/Application.hpp>
#include <Tether/Common/LibraryLoader.hpp>
#include <Tether/Module/Rendering/RenderingModule.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether;
using namespace Tether::Rendering;

bool RenderingModule::LoadVulkan()
{
	if (vulkan)
		return false;

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

	vulkan = new(std::nothrow) Storage::VulkanNative();
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

Vulkan::ErrorCode RenderingModule::InitVulkan(bool validationlayers)
{
	if (!LoadVulkan())
		return Vulkan::ErrorCode::VULKAN_LIB_NOT_FOUND;

	VkResult result = vulkan->instance.Init("TetherApp", "Tether",
		validationlayers);
	switch (result)
	{
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return Vulkan::ErrorCode::INCOMPATIBLE_DRIVER;
		break;

		case VK_ERROR_LAYER_NOT_PRESENT:
			return Vulkan::ErrorCode::LAYER_NOT_PRESENT;
		break;

		case VK_SUCCESS: break;
		default: return Vulkan::ErrorCode::UNKNOWN; break;
	}

	vulkan->initialized = true;
	return Vulkan::ErrorCode::SUCCESS;
}

Rendering::Storage::VulkanNative* RenderingModule::GetVulkanNative()
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
	{
		// Then dispose the instance
		vulkan->instance.Dispose();
	}

	// Dispose loaded stuff
	LibraryLoader::FreeLibrary(vulkan->handle);

	delete vulkan;
	vulkan = nullptr;
}
