#include <Tether/Tether.hpp>

#include <Tether/Renderer/Vulkan/RenderContextNative.hpp>

#include <iostream>
#include <vector>

#define TETHER_INCLUDE_VULKAN
#include <Tether/NativeVulkan.hpp>

using namespace Tether;

static bool InitVulkan(SimpleWindow* pWindow)
{
	/*if (!surface.Init(&instance, pWindow))
		return false;

	VkPhysicalDevice physicalDevice;
	if (!PickDevice(&physicalDevice, &surface))
		return false;

	if (!device.Init(&instance, physicalDevice, ))*/
	return true;
}

static void DisposeVulkan()
{
	/*surface.Dispose();*/
}

int main()
{
	Application& app = Application::Get();
	app.InitVulkan();

	SimpleWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	window.Hint(HintType::VISIBLE, false);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	if (!InitVulkan(&window))
	{
		std::cout << "Failed to initialize Vulkan" << std::endl;
		return 1;
	}

	Renderer::RenderContext ctx;
	Renderer::Vulkan::RenderContextNative vkNative;

	vkNative.Init();
	ctx.Init(&vkNative);

	window.SetVisible(true);
	while (!window.IsCloseRequested())
	{
		window.PollEvents();
	}

	window.Dispose();
	DisposeVulkan();
	return 0;
}