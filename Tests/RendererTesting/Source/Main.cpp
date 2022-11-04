#include <Tether/Tether.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleNative.hpp>

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

static SimpleWindow window;
static DebugLogger vulkanLogger;
static Renderer::RenderContext renderContext;
static Rendering::Vulkan::SimpleNative vkNative;

static bool InitVulkan()
{
	Rendering::RenderingModule& rendering = Rendering::RenderingModule::Get();

	// Initialize Vulkan. Doing so shouldn't be necessary since anything that needs
	// Vulkan initializes itself internally, but doing that you usually don't get
	// information about how it failed, so do it here instead.
	Rendering::Vulkan::ErrorCode error = rendering.InitVulkan(true);
	switch (error)
	{
		case Rendering::Vulkan::ErrorCode::INCOMPATIBLE_DRIVER:
		{
			std::cerr << "Incompatible Vulkan driver" << std::endl;
			return false;
		}
		break;

		case Rendering::Vulkan::ErrorCode::SUCCESS: break;
		default: return false;
	}

	rendering.GetVulkanNative()->instance.AddDebugMessenger(&vulkanLogger);

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
			std::cout << "Failed to initialize Vulkan." << std::endl;
			return false;
		}
		break;
	}
	
	renderContext.Init(&vkNative);

	return true;
}

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
	
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	window.Hint(HintType::VISIBLE, false);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 2;
	}

	if (!InitVulkan())
		return 3;

	window.SetVisible(true);

	std::chrono::high_resolution_clock::time_point startTime;
	while (!window.IsCloseRequested())
	{
		window.PollEvents();

		renderContext.RenderFrame();

		std::chrono::duration<float, std::milli> timeSpan =
			std::chrono::high_resolution_clock::now() - startTime;
		// std::cout << "FPS: " << 1000.0f / timeSpan.count() << std::endl;
		startTime = std::chrono::high_resolution_clock::now();
	}

	window.Dispose();
	renderContext.Dispose();
	app.Dispose();
	return 0;
}