#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanContext.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT VulkanWindow
	{
	public:
		VulkanWindow(VulkanContext& context, Window& window);
		~VulkanWindow();
		TETHER_NO_COPY(VulkanWindow);

		VkRenderPass Get();
	private:
		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;

		VkRenderPass m_RenderPass = nullptr;
	};
}