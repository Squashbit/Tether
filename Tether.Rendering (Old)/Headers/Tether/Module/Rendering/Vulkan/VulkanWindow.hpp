#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanWindow : public Context
	{
		friend class VulkanCompositor;
	public:
		VulkanWindow(Window& window);
		~VulkanWindow();
		TETHER_NO_COPY(VulkanWindow);
	protected:
		Window& window;
		Surface m_Surface;
		VkSurfaceFormatKHR m_SurfaceFormat;
		QueueFamilyIndices indices;
	private:
		void ChooseSurfaceFormat();
	};
}