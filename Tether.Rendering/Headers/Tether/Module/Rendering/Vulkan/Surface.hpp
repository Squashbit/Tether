#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class Instance;
	class InstanceLoader;
	class TETHER_EXPORT Surface
	{
	public:
		Surface(GraphicsContext& context, Window& window);
		~Surface();
		TETHER_NO_COPY(Surface);

		VkSurfaceKHR Get();
	private:
		VkInstance m_Instance = nullptr;
		const InstanceLoader& m_Loader;

		VkSurfaceKHR m_Surface = nullptr;
	};
}
