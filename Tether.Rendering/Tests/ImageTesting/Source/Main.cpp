#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>
#include <Tether/Math/Constants.hpp>

#include <Tether/Rendering/ImageLoader.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>

using namespace Tether;
using namespace Rendering;

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

static Stopwatch fpsTimer;
static Stopwatch deltaTimer;
static Stopwatch fullTime;

static const size_t numObjects = 10;
static const float lineSpacing = 0.1f;

static size_t frames = 0;
static float time = 0.0f;

static Scope<Window> s_Window = Window::Create(1280, 720, L"Renderer testing");
static Scope<Resources::BufferedImage> testImage;
static Scope<WindowRenderer> s_WindowRenderer;

static Vulkan::ContextCreator s_ContextCreator;
static DebugLogger vulkanLogger;

static void LoadResources(Vulkan::GraphicsContext& graphicsContext)
{
	ImageLoader imageLoader("Assets/Test.png");
	if (!imageLoader.Load())
		throw std::runtime_error("Failed to load image");

	testImage = graphicsContext.CreateBufferedImage(
		imageLoader.GetImageInfo());
}

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	DebugLogger vulkanLogger;
	s_ContextCreator.AddDebugMessenger(&vulkanLogger);

	Vulkan::GraphicsContext graphicsContext(s_ContextCreator);
	s_WindowRenderer = graphicsContext.CreateWindowRenderer(*s_Window);

	LoadResources(graphicsContext);

	s_Window->SetVisible(true);

	while (!s_Window->IsCloseRequested())
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

		int windowWidth = s_Window->GetWidth();
		int windowHeight = s_Window->GetHeight();
		const float imageWidth = (1.0f / numObjects) * windowWidth;
		const float imageHeight = (1.0f / numObjects) * windowHeight;

		Scope<RenderAction> render = s_WindowRenderer->StartRender();

		for (size_t i = 0; i < numObjects; i++)
		{
			float yTime = fullTime.GetElapsedSeconds() / 3;
			yTime += (numObjects - i) * 0.03f;

			float ypos = abs(sin(yTime * Math::PI));
			ypos *= 1 - lineSpacing;

			render->DrawImage(
				(i / (float)numObjects) * windowWidth,
				(1 - ypos - lineSpacing) * windowHeight,
				imageWidth, imageHeight,
				*testImage
			);
		}
	}

	return 0;
}