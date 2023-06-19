#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>
#include <Tether/Math/Constants.hpp>

#include <Tether/Rendering/ImageLoader.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ContextCreator.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;

class DebugLogger : public Rendering::Vulkan::DebugCallback
{
public:
	DebugLogger(Vulkan::ContextCreator& contextCreator)
	{
		contextCreator.AddDebugMessenger(this);
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
};

class ImageTesting
{
public:
	ImageTesting()
		:
		m_Window(Window::Create(1280, 720, L"Image testing")),
		m_VulkanLogger(m_ContextCreator),
		m_GraphicsContext(m_ContextCreator),
		m_WndRenderTarget(m_GraphicsContext.CreateWindowRenderTarget(*m_Window)),
		m_WindowRenderer(m_WndRenderTarget->GetRenderer())
	{
		LoadResources();
	}

	void Run()
	{
		m_Window->SetVisible(true);

		while (!m_Window->IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			Application::Get().PollEvents();

			time += delta;
			frames++;

			if (fpsTimer.GetElapsedSeconds() >= 3.0f)
			{
				std::cout << "FPS = " << 1.0f / (time / frames) << std::endl;

				time = 0;
				frames = 0;

				fpsTimer.Set();
			}

			int windowWidth = m_Window->GetWidth();
			int windowHeight = m_Window->GetHeight();
			const float imageWidth = (1.0f / numObjects) * windowWidth;
			const float imageHeight = (1.0f / numObjects) * windowHeight;

			m_WndRenderTarget->StartRender();

			for (size_t i = 0; i < numObjects; i++)
			{
				float yTime = fullTime.GetElapsedSeconds() / 3;
				yTime += (numObjects - i) * 0.03f;

				float ypos = abs(sin(yTime * Math::PI));
				ypos *= 1 - lineSpacing;

				m_WindowRenderer.DrawImage(
					(i / (float)numObjects) * windowWidth,
					(1 - ypos - lineSpacing) * windowHeight,
					imageWidth, imageHeight,
					*testImage
				);
			}

			m_WndRenderTarget->EndRender();
		}
	}
private:
	void LoadResources()
	{
		ImageLoader imageLoader("Assets/Test.png");
		if (!imageLoader.Load())
			throw std::runtime_error("Failed to load image");

		testImage = m_GraphicsContext.CreateBufferedImage(
			imageLoader.GetImageInfo());
	}

	Stopwatch fpsTimer;
	Stopwatch deltaTimer;
	Stopwatch fullTime;

	const size_t numObjects = 10;
	const float lineSpacing = 0.1f;

	size_t frames = 0;
	float time = 0.0f;

	Scope<Window> m_Window;

	DebugLogger m_VulkanLogger;
	Vulkan::ContextCreator m_ContextCreator;
	Vulkan::GraphicsContext m_GraphicsContext;

	Scope<RenderTarget> m_WndRenderTarget;
	Renderer& m_WindowRenderer;

	Scope<Resources::BufferedImage> testImage;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	ImageTesting app;
	app.Run();

	return 0;
}