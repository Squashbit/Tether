#pragma once

#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class VulkanContext
	{
	public:
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr;

		VkInstance instance = nullptr;
		InstanceLoader instanceLoader; // Must have had Load called
		VkDevice device = nullptr;
		VkQueue graphicsQueue = nullptr;
		DeviceLoader deviceLoader; // Must have had Load called
		VkPhysicalDevice physicalDevice = nullptr;
		VkRenderPass renderPass = nullptr;
		// If this is still nullptr, one will be created automatically.
		VmaAllocator allocator = nullptr;
		VkCommandPool commandPool = nullptr;
		uint32_t framesInFlight = 2;
	};
}