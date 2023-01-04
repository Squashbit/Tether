#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Allocator
	{
	public:
		Allocator(
			VkInstance instance, 
			VkDevice device, 
			VkPhysicalDevice physicalDevice, 
			InstanceLoader* iloader
		);
		~Allocator();

		VmaAllocator Get();
	private:
		VmaAllocator allocator;
	};
}