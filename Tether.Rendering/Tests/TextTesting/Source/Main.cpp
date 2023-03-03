#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanCompositor.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;
using namespace Vulkan;

class DebugLogger : public Rendering::Vulkan::DebugCallback
{
public:
	void OnDebugLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
	)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				std::cout << pCallbackData->pMessage << std::endl;
			}
			break;
		}
	}
};

class RendererTestApp
{
public:
	RendererTestApp()
		:
		m_Window(Window::Create(1280, 720, L"Text testing")),
		m_VulkanWindow(*m_Window),
		m_Renderer(m_VulkanWindow.MakeVulkanContext()),
		m_Compositor(m_Renderer, m_VulkanWindow)
	{
		font = m_Renderer.CreateResource<Resources::Font>("Assets/font.ttf");
		font->SetSize(64);

		text = m_Renderer.CreateObject<Objects::Text>();
		text->SetX(100.0f);
		text->SetY(100.0f);
		text->SetFont(font.get());
		text->SetText("FPS = 0");

		m_Renderer.AddObject(*text);

		m_Window->SetVisible(true);
	}

	void Run()
	{
		while (!m_Window->IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			time += delta;
			frames++;

			if (printFpsTimer.GetElapsedSeconds() >= 1.0f)
			{
				printFpsTimer.Set();

				int fps = (int)round(1.0f / (time / frames));
				text->SetText("FPS = " + std::to_string(fps));

				time = 0;
				frames = 0;
			}

			m_Window->PollEvents();
			m_Compositor.RenderFrame();
		}
	}
private:
	size_t frames = 0;
	float time = 0.0f;

	Scope<Window> m_Window;

	Vulkan::VulkanWindow m_VulkanWindow;
	Vulkan::VulkanRenderer m_Renderer;
	Vulkan::VulkanCompositor m_Compositor;

	Scope<Resources::Font> font;
	Scope<Objects::Text> text;

	Stopwatch printFpsTimer;
	Stopwatch deltaTimer;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	DebugLogger vulkanLogger;
	GlobalVulkan::Get().AddDebugMessenger(vulkanLogger);

	RendererTestApp testApp;
	testApp.Run();

	return 0;
}