#pragma once

#include <Tether/Window.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class Instance;
	class InstanceLoader;
	class TETHER_EXPORT Surface
	{
	public:
		Surface(VulkanContext& context, Window& window);
		~Surface();
		TETHER_NO_COPY(Surface);

		VkSurfaceKHR Get();
	private:
		VkInstance m_Instance = nullptr;
		InstanceLoader& m_Loader;

		VkSurfaceKHR m_Surface = nullptr;
	};
}
