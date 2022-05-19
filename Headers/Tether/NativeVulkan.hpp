#ifndef _TETHER_NATIVEVULKAN_HPP
#define _TETHER_NATIVEVULKAN_HPP

#include <Tether/Common/Defs.hpp>
#ifdef TETHER_INCLUDE_VULKAN

#include <Tether/Application.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Storage
{
#define TETHER_APP_VK Application::Get().GetVulkanNative()
	
#define vkGetInstanceProcAddr(instance, pName) \
	TETHER_APP_VK->GetInstanceProcAddr(instance, pName)
#define vkCreateInstance(createInfo, pAllocator, pInstance) \
	TETHER_APP_VK->CreateInstance(createInfo, pAllocator, pInstance)
#define vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties) \
	TETHER_APP_VK->EnumInstanceExtProps(pLayerName, pPropertyCount, pProperties)

	struct VulkanNative
	{
		void* handle = nullptr;
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
		// Global Commands
		PFN_vkCreateInstance CreateInstance;
		PFN_vkEnumerateInstanceExtensionProperties EnumInstanceExtProps;

		bool initialized = false;
	};
}

#endif // TETHER_INCLUDE_VULKAN
#endif //_TETHER_NATIVEVULKAN_HPP