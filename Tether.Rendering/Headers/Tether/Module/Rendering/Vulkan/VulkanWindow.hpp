#pragma once

#include <Tether/Window.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanWindow
	{
		friend class Renderer;
	public:
		VulkanWindow(Context& context, Window& window);
		~VulkanWindow();
		TETHER_NO_COPY(VulkanWindow);
	protected:
		VkRenderPass renderPass = nullptr;
		Window& window;
		Surface m_Surface;
		VkSurfaceFormatKHR m_SurfaceFormat;
		QueueFamilyIndices indices;
	private:
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;

		void ChooseSurfaceFormat(InstanceLoader& iloader,
			VkPhysicalDevice physicalDevice);
	};
}