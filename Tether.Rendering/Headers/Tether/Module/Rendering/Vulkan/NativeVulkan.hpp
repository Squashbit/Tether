// This file shouldn't usually be included in a header.

#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>

#include <optional>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{	
	struct TETHER_EXPORT VulkanNative
	{
		void* handle = nullptr;

		TETHER_VULKAN_FUNC_VAR(CreateInstance);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);
		
		std::optional<Vulkan::Instance> instance;
	};
}
