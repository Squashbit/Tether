#include <Tether/Tether.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleNative.hpp>

#include <Tether/Module/Rendering/Objects/Rectangle.hpp>

#include <iostream>
#include <vector>
#include <chrono>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether;

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
	Rendering::RenderingModule& rendering = Rendering::RenderingModule::Get();

#if defined(_WIN32) && !defined(_DEBUG)
	bool debug = false;
#else
	bool debug = true;
#endif

	// Initialize Vulkan. Doing so shouldn't be necessary since anything that needs
	// Vulkan initializes itself internally, but doing that you usually don't get
	// information about how it failed, so do it here instead.
	Rendering::Vulkan::ErrorCode error = rendering.InitVulkan(debug);
	switch (error)
	{
		case Rendering::Vulkan::ErrorCode::INCOMPATIBLE_DRIVER:
		{
			std::cerr << "Incompatible Vulkan driver" << std::endl;
			return false;
		}
		break;

		case Rendering::Vulkan::ErrorCode::SUCCESS: break;
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
		renderer(&vkNative),
		testRect(&renderer)
	{
		window.Hint(HintType::X, 120);
		window.Hint(HintType::Y, 120);
		window.Hint(HintType::VISIBLE, false);
		if (!window.Init(1280, 720, "Renderer testing"))
		{
			std::cout << "Failed to initialize window" << std::endl;
			return;
		}

		if (!InitRenderer())
		{
			std::cout << "Failed to initialize renderer" << std::endl;
			return;
		}

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
	bool InitRenderer()
	{
		Rendering::Vulkan::ErrorCode nativeError = vkNative.Init(&window);
		switch (nativeError)
		{
			case Rendering::Vulkan::ErrorCode::SUCCESS: break;

			case Rendering::Vulkan::ErrorCode::DEVICE_NOT_FOUND:
			{
				std::cout << "Failed to initialize Vulkan. No suitable device found."
					<< std::endl;
				return false;
			}
			break;

			default:
			{
				std::cout << "Failed to initialize Vulkan Native. Error code = "
					<< (int)nativeError << std::endl;
				return false;
			}
			break;
		}

		return true;
	}

	void InitObjects()
	{
		renderer.AddObject(&testRect);
	}

	void OnDispose()
	{
		window.Dispose();
		testRect.Dispose();
		renderer.Dispose();
	}
private:
	SimpleWindow window;

	Rendering::Vulkan::SimpleNative vkNative;
	Rendering::UIRenderer renderer;
	Rendering::Objects::Rectangle testRect;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,
	int nCmdShow)
#else
int main()
#endif
{
	Application& app = Application::Get();
	Rendering::RenderingModule& rendering = Rendering::RenderingModule::Get();

	// Initialize the application (wow really)
	app.Init();

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
	app.Dispose();
	return 0;
}