#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Application.hpp>
#include <Tether/Common/LibraryLoader.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>

#include <stdexcept>

#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

#define TETHER_INSTANCE_FUNC_NULL(name) \
	vulkan->name = (PFN_vk##name)vulkan->GetInstanceProcAddr(nullptr, "vk"#name)

namespace Tether::Rendering::Vulkan
{
	GlobalVulkan* GlobalVulkan::internal = nullptr;

	GlobalVulkan::GlobalVulkan(bool debugMode)
	{
		LoadVulkan();

		InstanceInfo info;
		info.CreateInstance = vulkan->CreateInstance;
		info.EnumerateInstanceExtensionProperties = vulkan->EnumerateInstanceExtensionProperties;
		info.EnumerateInstanceLayerProperties = vulkan->EnumerateInstanceLayerProperties;
		info.GetInstanceProcAddr = vulkan->GetInstanceProcAddr;

		vulkan->instance.emplace(info, debugMode);
	}

	GlobalVulkan::~GlobalVulkan()
	{
		vulkan->instance.reset();

		LibraryLoader::FreeLibrary(vulkan->handle);

		delete vulkan;
	}

	VulkanNative* GlobalVulkan::GetVulkanNative()
	{
		return vulkan;
	}

	void GlobalVulkan::AddDebugMessenger(DebugCallback* pDebugCallback)
	{
		pDebugCallback->m_IsInGlobalVulkan = true;
		vulkan->instance->AddDebugMessenger(pDebugCallback);
	}

	void GlobalVulkan::RemoveDebugMessenger(DebugCallback* pDebugCallback)
	{
		pDebugCallback->m_IsInGlobalVulkan = false;
		vulkan->instance->RemoveDebugMessenger(pDebugCallback);
	}

	void GlobalVulkan::LoadVulkan()
	{
		vulkan = new Vulkan::VulkanNative();

		void* vulkanLibrary;
#if defined(_WIN32)
		vulkanLibrary = LibraryLoader::LoadLibrary("vulkan-1.dll");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
		vulkanLibrary = LibraryLoader::LoadLibrary("libvulkan.so");
#else
		vulkanLibrary = LibraryLoader::LoadLibrary("libvulkan.so.1");
#endif

		if (!vulkanLibrary)
			throw std::runtime_error("Failed to load Vulkan library");
		
		vulkan->handle = vulkanLibrary;

		vulkan->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
			LibraryLoader::LoadFunction(
				vulkan->handle, "vkGetInstanceProcAddr");

		TETHER_INSTANCE_FUNC_NULL(CreateInstance);
		TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceExtensionProperties);
		TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceLayerProperties);
	}

	void GlobalVulkan::Create(bool debugMode)
	{
		if (internal)
			return;

		internal = new GlobalVulkan(debugMode);
	}

	bool GlobalVulkan::IsAlive()
	{
		return internal != nullptr;
	}

	void GlobalVulkan::Destroy()
	{
		delete internal;
		internal = nullptr;
	}

	GlobalVulkan& GlobalVulkan::Get()
	{
		if (!internal)
		{
#if defined(_DEBUG)
			Create(true);
#else
			Create(false);
#endif
		}

		return *internal;
	}
}