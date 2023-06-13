#pragma once

#include <Tether/Window.hpp>
#include <Tether/Rendering/WindowRenderer.hpp>
#include <Tether/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Rendering/GraphicsContext.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT WindowRendererVk : public Rendering::WindowRenderer,
		public VulkanRenderer
	{
	public:
		WindowRendererVk(GraphicsContext& context, Window& window);
		~WindowRendererVk();
		TETHER_NO_COPY(WindowRendererVk);

		void StartRender() override;
		void EndRender() override;
	private:
		GraphicsContext& m_Context;
		Window& m_Window;
	};
}