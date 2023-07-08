#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Rendering/Vulkan/ContextCreator.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;
using namespace Vulkan;

class DebugLogger : public Vulkan::DebugCallback
{
public:
	DebugLogger(Vulkan::ContextCreator& contextCreator)
		:
		m_ContextCreator(contextCreator)
	{
		m_ContextCreator.AddDebugMessenger(this);
	}

	~DebugLogger()
	{
		m_ContextCreator.RemoveDebugMessenger(this);
	}

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
private:
	Vulkan::ContextCreator& m_ContextCreator;
};

class RendererTestApp
{
public:
	RendererTestApp()
		:
		m_Window(Window::Create(1280, 720, L"Text testing")),
		m_VulkanLogger(m_ContextCreator),
		m_GraphicsContext(m_ContextCreator),
		m_WindowRenderTarget(m_GraphicsContext.CreateWindowRenderTarget(*m_Window)),
		m_WindowRenderer(m_WindowRenderTarget->GetRenderer())
	{
		m_Font = m_GraphicsContext.CreateFont("Assets/font.ttf");
		m_Font->SetSize(64);

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

			Application::Get().PollEvents();

			if (printFpsTimer.GetElapsedSeconds() >= 1.0f)
			{
				printFpsTimer.Set();

				int fps = (int)round(1.0f / (time / frames));
				m_FpsText = "FPS = " + std::to_string(fps);

				time = 0;
				frames = 0;
			}
			
			m_WindowRenderTarget->StartRender();
			{
				m_WindowRenderer.DrawText(100.0f, 100.0f, m_FpsText, *m_Font);
			}
			m_WindowRenderTarget->EndRender();
		}
	}
private:
	size_t frames = 0;
	float time = 0.0f;

	Scope<Window> m_Window;

	Vulkan::ContextCreator m_ContextCreator;
	DebugLogger m_VulkanLogger;
	Vulkan::GraphicsContext m_GraphicsContext;
	Scope<RenderTarget> m_WindowRenderTarget;
	Renderer& m_WindowRenderer;

	Scope<Resources::Font> m_Font;

	std::string m_FpsText = "FPS = 0";
	
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
	RendererTestApp testApp;
	testApp.Run();

	return 0;
}