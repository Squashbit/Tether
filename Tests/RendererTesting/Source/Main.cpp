#include <Tether/Tether.hpp>
#include <Tether/Renderer/Vulkan/RenderContextNative.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;
using namespace Tether;

int main()
{
	IWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	if (!window.Init(1280, 720, "Renderer testing"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	Renderer::RenderContext ctx;
	Renderer::Vulkan::RenderContextNative vkNative;

	vkNative.Init();
	ctx.Init(&vkNative);
	
	while (!window.IsCloseRequested())
	{
		window.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	return 0;
}