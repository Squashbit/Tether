#include <Tether/Module/Rendering/Vulkan/AllocatorManager.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	AllocatorManager::AllocatorManager(
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr,
		VkInstance instance,
		InstanceLoader instanceLoader,
		VkDevice device,
		VkPhysicalDevice physicalDevice
	)
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

		if (vmaCreateAllocator(&createInfo, &m_Allocator) != VK_SUCCESS)
			throw std::runtime_error("VMA allocator creation failed");
	}

	AllocatorManager::~AllocatorManager()
	{
		vmaDestroyAllocator(m_Allocator);
	}

	VmaAllocator AllocatorManager::Get()
	{
		return m_Allocator;
	}
}