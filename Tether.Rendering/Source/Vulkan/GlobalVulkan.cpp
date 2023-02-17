#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Common/LibraryLoader.hpp>

#include <stdexcept>

#define TETHER_INSTANCE_FUNC_NULL(name) \
	name = (PFN_vk##name)GetInstanceProcAddr(nullptr, "vk"#name)

namespace Tether::Rendering::Vulkan
{
	GlobalVulkan* GlobalVulkan::internal = nullptr;

	GlobalVulkan::GlobalVulkan(bool debugMode)
	{
		LoadVulkan();

		InstanceInfo info;
		info.CreateInstance = CreateInstance;
		info.EnumerateInstanceExtensionProperties = EnumerateInstanceExtensionProperties;
		info.EnumerateInstanceLayerProperties = EnumerateInstanceLayerProperties;
		info.GetInstanceProcAddr = GetInstanceProcAddr;

		instance.emplace(info, debugMode);
	}

	GlobalVulkan::~GlobalVulkan()
	{
		instance.reset();

		LibraryLoader::FreeLibrary(handle);
	}

	void GlobalVulkan::AddDebugMessenger(DebugCallback& debugCallback)
	{
		debugCallback.m_IsInGlobalVulkan = true;
		instance->AddDebugMessenger(debugCallback);
	}

	void GlobalVulkan::RemoveDebugMessenger(DebugCallback& debugCallback)
	{
		debugCallback.m_IsInGlobalVulkan = false;
		instance->RemoveDebugMessenger(debugCallback);
	}

	Instance& GlobalVulkan::GetInstance()
	{
		return *instance;
	}

	PFN_vkGetInstanceProcAddr GlobalVulkan::GetGetInstanceProcAddr()
	{
		return GetInstanceProcAddr;
	}

	void GlobalVulkan::LoadVulkan()
	{
#if defined(_WIN32)
		handle = LibraryLoader::LoadLibrary("vulkan-1.dll");
#elif defined(__OpenBSD__) || defined(__NetBSD__)
		handle = LibraryLoader::LoadLibrary("libvulkan.so");
#else
		handle = LibraryLoader::LoadLibrary("libvulkan.so.1");
#endif

		if (!handle)
			throw std::runtime_error("Failed to load Vulkan library");
		
		GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
			LibraryLoader::LoadFunction(
				handle, "vkGetInstanceProcAddr");

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