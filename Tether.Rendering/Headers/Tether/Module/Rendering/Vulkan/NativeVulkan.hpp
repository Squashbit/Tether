// This file shouldn't usually be included in a header.
// Make sure vulkan.h/hpp isn't included after this is included, because it will result
// in (a lot of) errors.

#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#ifdef TETHER_INCLUDE_VULKAN

#include <Tether/Module/Rendering/RenderingModule.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Swapchain.hpp>

#include <optional>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
#define vkCreateInstance(createInfo, pAllocator, pInstance) \
	TETHER_APP_VK->CreateInstance(createInfo, pAllocator, pInstance)
#define vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties) \
	TETHER_APP_VK->EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties)
#define vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties) \
	TETHER_APP_VK->EnumerateInstanceLayerProperties(pPropertyCount, pProperties)
#define vkGetInstanceProcAddr(instance, pName) \
	TETHER_APP_VK->GetInstanceProcAddr(instance, pName)
	
	struct TETHER_EXPORT VulkanNative
	{
		void* handle = nullptr;

		TETHER_VULKAN_FUNC_VAR(CreateInstance);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);
		
		bool initialized = false;
		// Everything below has to have InitVulkan called to use

		std::optional<Vulkan::Instance> instance;
	};
}

#endif // TETHER_INCLUDE_VULKAN
