#pragma once

#include <Tether/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Rendering/Vulkan/DeviceLoader.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	struct VulkanInfo
	{
		VkInstance instance = nullptr;
		InstanceLoader* instanceLoader = nullptr;
		DeviceLoader* deviceLoader = nullptr;
		VkDevice device = nullptr;
		VkQueue queue = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;
		VkRenderPass renderPass = nullptr;
		VmaAllocator allocator = nullptr;
		VkCommandPool commandPool = nullptr;
		uint32_t framesInFlight = 2;
	};
}