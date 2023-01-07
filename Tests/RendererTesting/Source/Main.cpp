#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>
#include <Tether/Math/Constants.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanUIRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/TypeNames.hpp>

#include <iostream>
#include <vector>
#include <chrono>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

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
			{
				std::cout << pCallbackData->pMessage << std::endl;
			}
			break;
		}
	}
};
static DebugLogger vulkanLogger;

bool InitVulkan()
{
	RenderingModule& rendering = RenderingModule::Get();

#if defined(_WIN32) && !defined(_DEBUG)
	bool debug = false;
#else
	bool debug = true;
#endif

	try
	{
		rendering.InitVulkan(debug);
	}
	catch (RendererException& e)
	{
		std::cout << "Failed to initialize Vulkan! " << e.what() << std::endl;
	}

	rendering.GetVulkanNative()->instance->AddDebugMessenger(&vulkanLogger);

	return true;
}

class RendererTestApp
{
public:
	RendererTestApp()
		:
		window(1280, 720, "Renderer testing"),
		renderer(&window)
	{
		rectangles.resize(numSquares);
		for (size_t i = 0; i < numSquares; i++)
		{
			rectangles[i] = renderer.CreateObject<Objects::Rectangle>();
			Objects::Rectangle* rect = rectangles[i].get();

			float brightness = (float)(i + 1) / (numSquares + 1);

			rect->SetWidth(1 / (float)numSquares);
			rect->SetHeight(0.1f);
			rect->SetX(i / (float)numSquares);
			rect->SetColor(Color(0.0f, 0.6f * brightness, brightness));
			
			renderer.AddObject(rect);
		}

		window.SetVisible(true);
	}

	void Run()
	{
		while (!window.IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			time += delta;
			frames++;

			if (fpsTimer.GetElapsedSeconds() >= 3.0f)
			{
				std::cout << "FPS = " << 1.0f / (time / frames) << std::endl;

				time = 0;
				frames = 0;

				fpsTimer.Set();
			}

			for (size_t i = 0; i < numSquares; i++)
			{
				Objects::Rectangle* rect = rectangles[i].get();

				float rectTime = fullTime.GetElapsedSeconds() / 3;
				rectTime += (numSquares - i) * 0.03f;

				float timeSine = abs(sin(rectTime * Math::PI));
				timeSine *= 1 - rect->GetWidth();

				rect->SetY(1 - timeSine - rect->GetHeight());
			}

			window.PollEvents();
			renderer.RenderFrame();
		}
	}
private:
	size_t numSquares = 10;

	size_t frames = 0;
	float time = 0.0f;

	SimpleWindow window;
	Vulkan::VulkanUIRenderer renderer;
	
	std::vector<Scope<Objects::Rectangle>> rectangles;

	Stopwatch fpsTimer;
	Stopwatch deltaTimer;
	Stopwatch fullTime;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	if (!InitVulkan())
	{
		std::cout << "Failed to initialize Vulkan" << std::endl;
		return 1;
	}

	try
	{
		RendererTestApp testApp;
		testApp.Run();
	}
	catch (RendererException& e)
	{
		std::cout << e.what() << std::endl;
	}

	Application::DisposeApplication();
	return 0;
}