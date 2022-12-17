#include <Tether/Tether.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanUIRenderer.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <iostream>
#include <vector>
#include <chrono>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

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

	// Initialize Vulkan. Doing so shouldn't be necessary since anything that needs
	// Vulkan initializes itself internally, but doing that you usually don't get
	// information about how it failed, so do it here instead.
	Vulkan::ErrorCode error = rendering.InitVulkan(debug);
	switch (error)
	{
		case Vulkan::ErrorCode::INCOMPATIBLE_DRIVER:
		{
			std::cerr << "Incompatible Vulkan driver" << std::endl;
			return false;
		}
		break;

		case Vulkan::ErrorCode::SUCCESS: break;
		default:
		{
			std::cout << "Failed to initialize Vulkan. Error code = "
				<< (int)error << std::endl;
			return false;
		}
		break;
	}

	rendering.GetVulkanNative()->instance.AddDebugMessenger(&vulkanLogger);

	return true;
}

class RendererTestApp : public IDisposable
{
public:
	TETHER_DISPOSE_ON_DESTROY(RendererTestApp);
	RendererTestApp()
		:
		window(1280, 720, "Renderer testing"),
		renderer(&window)
	{
		InitObjects();

		window.SetVisible(true);

		initialized = true;
	}

	void Run()
	{
		while (!window.IsCloseRequested())
		{
			window.PollEvents();
			renderer.RenderFrame();
		}
	}
private:
	void InitObjects()
	{
		testRect = ScopeTools::Create<Objects::Rectangle>(&renderer);
		renderer.AddObject(testRect.get());
	}

	void OnDispose()
	{
		window.Dispose();
		testRect->Dispose();
		renderer.Dispose();
	}
private:
	SimpleWindow window;

	Vulkan::VulkanUIRenderer renderer;
	Scope<Objects::Rectangle> testRect;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,
	int nCmdShow)
#else
int main()
#endif
{
	if (!InitVulkan())
	{
		std::cout << "Failed to initialize Vulkan" << std::endl;
		return 1;
	}

	RendererTestApp testApp;
	if (!testApp.IsInitialized())
		return 2;

	testApp.Run();

	testApp.Dispose();
	Application::DisposeApplication();
	return 0;
}