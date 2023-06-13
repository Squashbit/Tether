#include <Tether/Rendering/Vulkan/WindowRendererVk.hpp>

namespace Tether::Rendering::Vulkan
{
	WindowRendererVk::WindowRendererVk(GraphicsContext& context,
		Window& window)
		:
		VulkanRenderer(context),
		m_Context(context),
		m_Window(window)
	{}

	WindowRendererVk::~WindowRendererVk()
	{

	}
}