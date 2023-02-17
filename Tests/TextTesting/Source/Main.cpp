#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleRenderer.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;
using namespace Vulkan;

class DebugLogger : public DebugCallback
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
		window(1280, 720, "Text testing"),
		m_VulkanContext(window),
		m_Renderer(window, m_VulkanContext)
	{
		font = m_Renderer.CreateResource<Resources::Font>("font.ttf");
		font->SetSize(64);

		text = m_Renderer.CreateObject<Objects::Text>();
		text->SetX(0.05f);
		text->SetY(0.5f);
		text->SetFont(font.get());
		text->SetText("FPS = 0");
		
		m_Renderer.AddObject(text.get());
		m_Renderer.SetClearColor(0.2f);

		window.SetVisible(true);
	}

	~RendererTestApp()
	{
		window.SetVisible(false);
	}

	void Run()
	{
		size_t frames = 0;
		float time = 0.0f;
		while (!window.IsCloseRequested())
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

			window.PollEvents();
			m_Renderer.RenderFrame();
		}
	}
private:
	SimpleWindow window;

	SimpleVulkanContext m_VulkanContext;
	SimpleRenderer m_Renderer;

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