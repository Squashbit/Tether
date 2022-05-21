#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class TestWindow : public Tether::GraphicalWindow
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(TestWindow* pWindow)
			:
			pWindow(pWindow)
		{}

		void OnWindowClosing(Events::WindowClosingEvent event)
		{
			pWindow->SetVisible(false);
			std::cout << "Closed window" << std::endl;
		}
		
		void OnWindowError(Events::WindowErrorEvent event)
		{
			std::cout << "window error: " << std::endl;
			std::cout << "\tERROR    = " << (int)event.GetCode() << std::endl;
			std::cout << "\tSEVERITY = " << (int)event.GetSeverity() 
				<< std::endl;
			std::cout << "\tFUNC_NAME = " << event.GetFunctionName() 
				<< std::endl;
		}
	private:
		TestWindow* pWindow = nullptr;
	};

	TestWindow()
		:
		handler(this)
	{
	}

	~TestWindow()
	{
		RemoveEventHandler(handler);
	}

	void OnInit()
	{
		SetBounds(960, 540, 1366, 768);
		
		AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		AddEventHandler(handler, Events::EventType::WINDOW_ERROR);

		SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));
	}
private:
	EventHandler handler;
};

int main()
{
	TestWindow window;
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	if (!window.Init(1280, 720, "GraphicalWindow 1"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	TestWindow window2;
	window2.Hint(HintType::X, 240);
	window2.Hint(HintType::Y, 240);
	if (!window2.Init(1280, 720, "GraphicalWindow 2"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	while (!window.IsCloseRequested() || !window2.IsCloseRequested())
	{
		window.PollEvents();
		window2.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	window2.Dispose();
	return 0;
}