#include <Tether/Tether.hpp>

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
	
	while (!window.IsCloseRequested())
	{
		window.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	return 0;
}