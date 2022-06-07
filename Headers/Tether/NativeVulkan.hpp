// This file shouldn't usually be included in a header.
// Make sure vulkan.h/hpp isn't included after this is included, because it will result
// in (a lot of) errors.

#ifndef _TETHER_NATIVEVULKAN_HPP
#define _TETHER_NATIVEVULKAN_HPP

#include <Tether/Common/Defs.hpp>
#ifdef TETHER_INCLUDE_VULKAN

#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Renderer/Vulkan/InstanceLoader.hpp>
#include <Tether/Renderer/Vulkan/Surface.hpp>
#include <Tether/Renderer/Vulkan/Device.hpp>
#include <Tether/Renderer/Vulkan/Swapchain.hpp>

#include <Tether/Application.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Storage
{
#define vkCreateInstance(createInfo, pAllocator, pInstance) \
	TETHER_APP_VK->CreateInstance(createInfo, pAllocator, pInstance)
#define vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties) \
	TETHER_APP_VK->EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties)
#define vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties) \
	TETHER_APP_VK->EnumerateInstanceLayerProperties(pPropertyCount, pProperties)
#define vkGetInstanceProcAddr(instance, pName) \
	TETHER_APP_VK->GetInstanceProcAddr(instance, pName)
	
	struct VulkanNative
	{
		void* handle = nullptr;

		TETHER_VULKAN_FUNC_VAR(CreateInstance);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
		TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);
		
		bool initialized = false;
		// Everything below has to have InitVulkan called to use

		Vulkan::Instance instance;
	};
}

#endif // TETHER_INCLUDE_VULKAN
#endif //_TETHER_NATIVEVULKAN_HPP