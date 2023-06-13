#pragma once

#include <Tether/Window.hpp>
#include <Tether/Rendering/WindowRenderer.hpp>
#include <Tether/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Rendering/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Rendering/Vulkan/Surface.hpp>

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
		void ChooseSurfaceFormat();

		GraphicsContext& m_Context;
		Window& m_Window;

		VkDevice m_Device = nullptr;
		const InstanceLoader& m_Iloader;
		const DeviceLoader& m_Dloader;

		Surface m_Surface;
		VkSurfaceFormatKHR m_SurfaceFormat;
		QueueFamilyIndices indices;
		VkRenderPass renderPass = nullptr;
	};
}