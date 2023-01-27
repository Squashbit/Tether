#include <Tether/Tether.hpp>
#include <Tether/Common/Stopwatch.hpp>

#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <random>

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
		window(1280, 720, "Renderer testing"),
		renderer(&window),
		gen(rd()),
		distr(0.0f, 1.0f)
	{
		objects.resize(1024);
		window.SetVisible(true);
	}

	~RendererTestApp()
	{
		window.SetVisible(false);
	}

	void Run()
	{
		while (!window.IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedSeconds();
			deltaTimer.Set();

			time += delta;
			frames++;

			AddObject();

			if (fpsTimer.GetElapsedSeconds() >= 3.0f)
			{
				std::cout << "FPS = " << 1.0f / (time / frames) << ", Objects = " 
					<< objectCount << '\n';

				time = 0;
				frames = 0;

				fpsTimer.Set();
			}

			window.PollEvents();
			renderer.RenderFrame();
		}
	}
private:
	void AddObject()
	{
		if (objectCount + 1 > objects.size())
			objects.resize(objects.size() + 1024);

		objects[objectCount] = renderer.CreateObject<Objects::Rectangle>();
		Objects::Rectangle* newRect = objects[objectCount].get();
		
		newRect->SetX(distr(gen));
		newRect->SetY(distr(gen));
		newRect->SetWidth(distr(gen));
		newRect->SetHeight(distr(gen));
		newRect->SetColor(distr(gen));

		renderer.AddObject(newRect);

		objectCount++;
	}

	size_t frames = 0;
	float time = 0.0f;

	SimpleWindow window;
	Vulkan::VulkanRenderer renderer;

	size_t objectCount = 0;
	std::vector<Scope<Objects::Rectangle>> objects;

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> distr;

	Stopwatch fpsTimer;
	Stopwatch deltaTimer;
	Stopwatch fullTime;
};

#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	DebugLogger vulkanLogger;
	GlobalVulkan::Get().AddDebugMessenger(&vulkanLogger);

	try
	{
		RendererTestApp testApp;
		testApp.Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}