#include <Tether/Module/Rendering/Vulkan/Context.hpp>
#include <Tether/Common/LibraryLoader.hpp>

#include <stdexcept>

#define TETHER_INSTANCE_FUNC_NULL(name) \
	name = (PFN_vk##name)GetInstanceProcAddr(nullptr, "vk"#name)

namespace Tether::Rendering::Vulkan
{
	Context::Context(bool debugMode)
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
		indices = m_Instance->FindQueueFamilies(physicalDevice);
		queue = m_Device->GetDeviceQueue(indices.graphicsFamilyIndex, 0);

		CreateAllocator();
		CreateCommandPool();
	}

	Context::~Context()
	{
		deviceLoader.vkDestroyCommandPool(m_Device->Get(), commandPool, nullptr);

		vmaDestroyAllocator(allocator);
		
		m_Device.reset();
		m_Instance.reset();

		LibraryLoader::FreeLibrary(handle);
	}

	void Context::AddDebugMessenger(DebugCallback& debugCallback)
	{
		if (&debugCallback.m_Context != this)
			throw std::invalid_argument("DebugCallback created with another context");

		debugCallback.m_IsInContext = true;
		m_Instance->AddDebugMessenger(debugCallback);
	}

	void Context::RemoveDebugMessenger(DebugCallback& debugCallback)
	{
		if (&debugCallback.m_Context != this)
			throw std::invalid_argument("DebugCallback created with another context");

		debugCallback.m_IsInContext = false;
		m_Instance->RemoveDebugMessenger(debugCallback);
	}

	void Context::LoadVulkan()
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

	void Context::CreateAllocator()
	{
		VmaVulkanFunctions funcs{};
		funcs.vkGetInstanceProcAddr = GetInstanceProcAddr;
		funcs.vkGetDeviceProcAddr = instanceLoader.vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.physicalDevice = physicalDevice;
		createInfo.device = device;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.instance = instance;
		createInfo.pVulkanFunctions = &funcs;

		if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS)
			throw std::runtime_error("VMA allocator creation failed");
	}

	void Context::CreateCommandPool()
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = indices.graphicsFamilyIndex;

		if (deviceLoader.vkCreateCommandPool(m_Device->Get(), &createInfo, nullptr,
			&commandPool) != VK_SUCCESS)
			throw std::runtime_error("Command pool creation failed");
	}
}