#ifndef _TETHER_VKUTILS_HPP
#define _TETHER_VKUTILS_HPP

#include <Tether/Common/Defs.hpp>
#ifdef TETHER_INCLUDE_VULKAN

#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan::VkUtils
{
	bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Instance* pInstance, 
		Vulkan::Surface* pSurface);
}

#endif // TETHER_INCLUDE_VULKAN
#endif //_TETHER_VKUTILS_HPP