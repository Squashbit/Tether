#include <Tether/Module/Rendering/Vulkan/SimpleVulkanContext.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	SimpleVulkanContext::SimpleVulkanContext()
		:
		m_Instance(GlobalVulkan::Get().GetInstance()),
		m_Device(m_Instance)
	{
		GetInstanceProcAddr = GlobalVulkan::Get().GetGetInstanceProcAddr();

		instance = m_Instance.Get();
		instanceLoader = m_Instance.GetLoader();
		device = m_Device.Get();
		deviceLoader = m_Device.GetLoader();
		physicalDevice = m_Device.GetPhysicalDevice();
		queueIndices = m_Instance.FindQueueFamilies(physicalDevice);
		graphicsQueue = m_Device.GetDeviceQueue(queueIndices.graphicsFamilyIndex, 0);
		
		CreateCommandPool();
	}

	SimpleVulkanContext::~SimpleVulkanContext()
	{
		deviceLoader.vkDestroyCommandPool(device, commandPool, nullptr);
		deviceLoader.vkDestroyRenderPass(device, renderPass, nullptr);
	}
	void SimpleVulkanContext::CreateCommandPool()
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