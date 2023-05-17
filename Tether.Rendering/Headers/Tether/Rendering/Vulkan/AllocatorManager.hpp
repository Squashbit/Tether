#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/InstanceLoader.hpp>

#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT AllocatorManager
	{
	public:
		AllocatorManager(
			PFN_vkGetInstanceProcAddr GetInstanceProcAddr,
			VkInstance instance,
			InstanceLoader instanceLoader,
			VkDevice device,
			VkPhysicalDevice physicalDevice
		);

		~AllocatorManager();
		TETHER_NO_COPY(AllocatorManager);

		VmaAllocator Get();
	private:
		VmaAllocator m_Allocator = nullptr;
	};
}