#include <Tether/Module/Rendering/Vulkan/AllocatorManager.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	AllocatorManager::AllocatorManager(Context& context)
		:
		m_Context(context)
	{
		VmaVulkanFunctions funcs{};
		funcs.vkGetInstanceProcAddr = m_Context.GetInstanceProcAddr;
		funcs.vkGetDeviceProcAddr = m_Context.instanceLoader.vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo createInfo{};
		createInfo.physicalDevice = m_Context.physicalDevice;
		createInfo.device = m_Context.device;
		createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		createInfo.instance = m_Context.instance;
		createInfo.pVulkanFunctions = &funcs;

		if (vmaCreateAllocator(&createInfo, &m_Context.allocator) != VK_SUCCESS)
			throw std::runtime_error("VMA allocator creation failed");
	}

	AllocatorManager::~AllocatorManager()
	{
		vmaDestroyAllocator(m_Context.allocator);
	}
}