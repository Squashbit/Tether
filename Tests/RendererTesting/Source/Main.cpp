#include <Tether/Tether.hpp>
#include <Tether/Module/Rendering/Vulkan/SimpleNative.hpp>

#include <iostream>
#include <vector>

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

int main()
{
	Application& app = Application::Get();
	Rendering::RenderingModule& rendering = Rendering::RenderingModule::Get();

	// Initialize the application
	app.Init();

	// Initialize Vulkan. Doing so shouldn't be necessary since anything that needs
	// Vulkan initializes itself internally, but doing that you usually don't get
	// information about how it failed, so do it here instead.
	Rendering::Vulkan::ErrorCode error = rendering.InitVulkan(true);
	switch (error)
	{
		case Rendering::Vulkan::ErrorCode::INCOMPATIBLE_DRIVER:
		{
			std::cerr << "Incompatible Vulkan driver" << std::endl;
			return 1;
		}
		break;
		
		case Rendering::Vulkan::ErrorCode::SUCCESS: break;
		default: return 1;
	}

	DebugLogger vulkanLogger;
	rendering.GetVulkanNative()->instance.AddDebugMessenger(&vulkanLogger);
	
	SimpleWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	window.Hint(HintType::VISIBLE, false);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 2;
	}

	Renderer::RenderContext ctx;
	Rendering::Vulkan::SimpleNative vkNative;

	window.SetVisible(true);
	if (vkNative.Init(&window) != Rendering::Vulkan::ErrorCode::SUCCESS)
		return 3;

	ctx.Init(&vkNative);

	while (!window.IsCloseRequested())
	{
		window.PollEvents();
	}

	window.Dispose();

	rendering.DisposeVulkan();
	return 0;
}