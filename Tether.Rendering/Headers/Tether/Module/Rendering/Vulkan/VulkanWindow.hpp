#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>

#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanWindow
	{
		friend class Compositor;
	public:
		VulkanWindow(Window& window, VulkanContext& context = GlobalVulkan::Get());
		~VulkanWindow();
		TETHER_NO_COPY(VulkanWindow);
	protected:
		VulkanContext& m_Context;

		Window& window;
		Surface m_Surface;
		VkSurfaceFormatKHR m_SurfaceFormat;
		QueueFamilyIndices indices;
		VkRenderPass renderPass = nullptr;
	private:
		void ChooseSurfaceFormat();
	};
}