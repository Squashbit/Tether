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

		m_Instance.emplace(info, debugMode);
		m_Device.emplace(*m_Instance);

		instance = m_Instance->Get();
		instanceLoader = m_Instance->GetLoader();
		device = m_Device->Get();
		deviceLoader = m_Device->GetLoader();
		physicalDevice = m_Device->GetPhysicalDevice();
		queueIndices = m_Instance->FindQueueFamilies(physicalDevice);
		queue = m_Device->GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);

		CreateCommandPool();
	}

	GlobalVulkan::~GlobalVulkan()
	{
		deviceLoader.vkDestroyCommandPool(device, commandPool, nullptr);
		
		m_Device.reset();
		m_Instance.reset();

		LibraryLoader::FreeLibrary(handle);
	}

	void GlobalVulkan::AddDebugMessenger(DebugCallback& debugCallback)
	{
		debugCallback.m_IsInGlobalVulkan = true;
		m_Instance->AddDebugMessenger(debugCallback);
	}

	void GlobalVulkan::RemoveDebugMessenger(DebugCallback& debugCallback)
	{
		debugCallback.m_IsInGlobalVulkan = false;
		m_Instance->RemoveDebugMessenger(debugCallback);
	}

	Instance& GlobalVulkan::GetInstance()
	{
		return *m_Instance;
	}

	const QueueFamilyIndices& GlobalVulkan::GetQueueFamilyIndices() const
	{
		return queueIndices;
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

	void GlobalVulkan::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = queueIndices.graphicsFamilyIndex;

		if (deviceLoader.vkCreateCommandPool(device, &createInfo, nullptr,
			&commandPool) != VK_SUCCESS)
			throw std::runtime_error("Command pool creation failed");
	}
}