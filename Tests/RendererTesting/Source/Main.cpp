#include <Tether/Tether.hpp>
#include <Tether/Renderer/Vulkan/SimpleNative.hpp>

#include <iostream>
#include <vector>

#define TETHER_INCLUDE_VULKAN
#include <Tether/NativeVulkan.hpp>

using namespace Tether;

class DebugLogger : public Vulkan::DebugCallback
{
public:
	void OnDebugLog(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
	)
	{
		std::cout << pCallbackData->pMessage << std::endl;
		// switch (messageSeverity)
		// {
		// 	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		// 	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		// 	{
		// 		std::cout << pCallbackData->pMessage << std::endl;
		// 	}
		// 	break;
		// }
	}
};

int main()
{
	Application& app = Application::Get();
	app.InitVulkan(true);

	DebugLogger vulkanLogger;
	app.GetVulkanNative()->instance.AddDebugMessenger(&vulkanLogger);

	SimpleWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	window.Hint(HintType::VISIBLE, false);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	Renderer::RenderContext ctx;
	Vulkan::SimpleNative vkNative;

	window.SetVisible(true);
	if (vkNative.Init(&window) != Vulkan::ErrorCode::SUCCESS)
		return 1;

	ctx.Init(&vkNative);

	while (!window.IsCloseRequested())
	{
		window.PollEvents();
	}

	window.Dispose();
	return 0;
}