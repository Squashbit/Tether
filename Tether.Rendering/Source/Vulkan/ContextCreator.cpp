#include <Tether/Rendering/Vulkan/ContextCreator.hpp>

#define TETHER_INSTANCE_FUNC_NULL(name) \
	name = (PFN_vk##name)GetInstanceProcAddr(nullptr, "vk"#name)

namespace Tether::Rendering::Vulkan
{
	ContextCreator::VulkanLibrary::VulkanLibrary()
		:
#if defined(_WIN32)
		Library("vulkan-1.dll")
#elif defined(__OpenBSD__) || defined(__NetBSD__)
		Library("libvulkan.so")
#else
		Library("libvulkan.so.1")
#endif
	{
		if (!GetHandle())
			throw std::runtime_error("Failed to load Vulkan library");

		GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)
			LoadFunction("vkGetInstanceProcAddr");

		TETHER_INSTANCE_FUNC_NULL(CreateInstance);
		TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceExtensionProperties);
		TETHER_INSTANCE_FUNC_NULL(EnumerateInstanceLayerProperties);
	}

	ContextCreator::ContextCreator(bool debugMode)
	{
		InstanceInfo info;
		info.CreateInstance = m_VulkanLibrary.CreateInstance;
		info.GetInstanceProcAddr = m_VulkanLibrary.GetInstanceProcAddr;
		info.EnumerateInstanceExtensionProperties =
			m_VulkanLibrary.EnumerateInstanceExtensionProperties;
		info.EnumerateInstanceLayerProperties =
			m_VulkanLibrary.EnumerateInstanceLayerProperties;

		m_Instance.emplace(info, debugMode);
		m_Device.emplace(*m_Instance);

		QueueFamilyIndices queueIndices = 
			m_Instance->FindQueueFamilies(m_Device->GetPhysicalDevice());

		m_Queue = m_Device->GetDeviceQueue(queueIndices.graphicsFamilyIndex,
			0);

		CreateCommandPool(queueIndices.graphicsFamilyIndex);
	}

	ContextCreator::~ContextCreator()
	{
		GetDeviceLoader().vkDestroyCommandPool(m_Device->Get(), m_CommandPool, 
			nullptr);
	}

	void ContextCreator::AddDebugMessenger(DebugCallback* debugCallback)
	{
		m_Instance->AddDebugMessenger(*debugCallback);
	}

	void ContextCreator::RemoveDebugMessenger(DebugCallback* debugCallback)
	{
		m_Instance->RemoveDebugMessenger(*debugCallback);
	}

	const uint32_t ContextCreator::GetFramesInFlight() const
	{
		return m_FramesInFlight;
	}

	const InstanceLoader& ContextCreator::GetInstanceLoader() const
	{
		return m_Instance->GetLoader();
	}

	const DeviceLoader& ContextCreator::GetDeviceLoader() const
	{
		return m_Device->GetLoader();
	}

	const ContextCreator::VulkanLibrary& ContextCreator::GetVulkanLibrary() const
	{
		return m_VulkanLibrary;
	}

	VkInstance ContextCreator::GetInstance() const
	{
		return m_Instance->Get();
	}

	VkDevice ContextCreator::GetDevice() const
	{
		return m_Device->Get();
	}

	VkQueue ContextCreator::GetQueue() const
	{
		return m_Queue;
	}

	VkPhysicalDevice ContextCreator::GetPhysicalDevice() const
	{
		return m_Device->GetPhysicalDevice();
	}

	VkCommandPool ContextCreator::GetCommandPool() const
	{
		return m_CommandPool;
	}

	void ContextCreator::CreateCommandPool(uint32_t graphicsFamilyIndex)
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = graphicsFamilyIndex;

		if (GetDeviceLoader().vkCreateCommandPool(m_Device->Get(), &createInfo,
			nullptr, &m_CommandPool) != VK_SUCCESS)
			throw std::runtime_error("Command pool creation failed");
	}
}