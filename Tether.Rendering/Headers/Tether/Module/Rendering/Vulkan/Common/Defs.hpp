#pragma once

#define TETHER_VULKAN_FUNC_VAR(name) PFN_vk##name name

#define TETHER_VULKAN_FUNC(name) \
	vulkan->name = (PFN_vk##name)LoadFunction(vulkan->handle, "vk"#name)

#define TETHER_APP_VK Tether::Rendering::Vulkan::GlobalVulkan::Get().GetVulkanNative()

#if defined(__INTELLISENSE__)

#if !defined(TETHER_INCLUDE_VULKAN)
#define TETHER_INCLUDE_VULKAN
#endif

#endif // __INTELLISENSE__
