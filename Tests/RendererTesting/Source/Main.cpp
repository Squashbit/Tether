#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>
#include <Tether/Math/Constants.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>
#include <Tether/Module/Rendering/Objects/Image.hpp>

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
		const float imageSize = 1.0f / numObjects;

		objects.resize(numObjects);
		for (size_t i = 0; i < numObjects; i++)
		{
			objects[i] = renderer.CreateObject<Objects::Image>();
			Objects::Image* image = objects[i].get();

			image->SetX(i / (float)numObjects);
			image->SetWidth(imageSize);
			image->SetHeight(imageSize);
			
			renderer.AddObject(image);
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

			for (size_t i = 0; i < numObjects; i++)
			{
				Objects::Image* image = objects[i].get();

				float x1time = fullTime.GetElapsedSeconds() / 3;
				float x2time = x1time;
				x1time += (numObjects - i) * 0.03f;
				
				float x1sine = abs(sin(x1time * Math::PI));
				x1sine *= 1 - lineSpacing;

				image->SetY(1 - x1sine - lineSpacing);
			}

			window.PollEvents();
			renderer.RenderFrame();
		}
	}
private:
	size_t numObjects = 10;
	float lineSpacing = 0.1f;

	size_t frames = 0;
	float time = 0.0f;

	SimpleWindow window;
	Vulkan::VulkanUIRenderer renderer;
	
	std::vector<Scope<Objects::Image>> objects;

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