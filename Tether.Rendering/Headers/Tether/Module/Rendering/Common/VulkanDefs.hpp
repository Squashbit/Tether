#ifndef _TETHER_VULKANDEFS_HPP
#define _TETHER_VULKANDEFS_HPP

#define TETHER_VULKAN_FUNC_VAR(name) PFN_vk##name name
#define TETHER_INSTANCE_FUNC_VAR(name) PFN_vk##name vk##name
#define TETHER_DEVICE_FUNC_VAR(name) TETHER_INSTANCE_FUNC_VAR(name)

#define TETHER_VULKAN_FUNC(name) \
	vulkan->name = (PFN_vk##name)LoadFunction(vulkan->handle, "vk"#name)

// Not to be confused with the _VAR equivalent.
// These load the functions. The other ones are variable macros.
#define TETHER_INSTANCE_FUNC(name) \
	vk##name = (PFN_vk##name)vkGetInstanceProcAddr(*pInstance, "vk"#name)
#define TETHER_DEVICE_FUNC(name) \
	vk##name = (PFN_vk##name)iloader->vkGetDeviceProcAddr(*pDevice, "vk"#name)

#define TETHER_INSTANCE_FUNC_NULL(name) \
	vulkan->name = (PFN_vk##name)vkGetInstanceProcAddr(nullptr, "vk"#name)

#define TETHER_APP_VK Tether::Rendering::RenderingModule::Get().GetVulkanNative()

#if defined(__INTELLISENSE__)

#if !defined(TETHER_INCLUDE_VULKAN)
#define TETHER_INCLUDE_VULKAN
#endif

#endif // __INTELLISENSE__

#endif //_TETHER_VULKANDEFS_HPP