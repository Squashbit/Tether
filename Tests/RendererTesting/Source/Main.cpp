#include <Tether/Tether.hpp>
#include <Tether/Renderer/Vulkan/RenderContextNative.hpp>

#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Renderer/Vulkan/Surface.hpp>
#include <Tether/Renderer/Vulkan/Device.hpp>
#include <Tether/Renderer/Vulkan/Swapchain.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;
using namespace Tether;

static Vulkan::Instance instance;
static Vulkan::Surface surface;
static Vulkan::Device device;
static Vulkan::Swapchain swapchain;

bool InitVulkan()
{
	return instance.Init("TestApp", "TestEngine", false);
}

void DisposeVulkan()
{
	instance.Dispose();
}

int main()
{
	SimpleWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	window.Hint(HintType::VISIBLE, false);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	if (!InitVulkan())
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
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	DisposeVulkan();
	return 0;
}