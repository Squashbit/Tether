#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class TestWindow : public Tether::Window
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(TestWindow* pWindow)
			:
			pWindow(pWindow)
		{}

		void OnWindowResize(Events::WindowResizeEvent event)
		{
			std::cout << "Resized window to W=" << event.GetNewWidth()
				<< ", H=" << event.GetNewHeight() << std::endl;
		}

		void OnWindowClosing(Events::WindowClosingEvent event)
		{
			pWindow->SetVisible(false);
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
		AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		AddEventHandler(handler, Events::EventType::WINDOW_ERROR);
		AddEventHandler(handler, Events::EventType::WINDOW_RESIZE);
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
	if (!window.Init(1280, 720, "sup"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	while (!window.IsCloseRequested())
	{
		window.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	return 0;
}