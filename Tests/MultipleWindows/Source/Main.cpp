#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class TestWindow : public Window
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

		void OnWindowMove(Events::WindowMoveEvent event)
		{
			std::cout << "Moved window to X=" << event.GetX()
				<< ", Y=" << event.GetY() << std::endl;
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

	class TestListener : public Input::InputListener
	{
	public:
		void OnMouseMove(Input::MouseMoveInfo& info)
		{
			std::cout << "Mouse move ("
				<< "relX=" << info.GetRelativeX() << ", "
				<< "relY=" << info.GetRelativeY()
				<< ")"
				<< std::endl;
		}

		void OnRawMouseMove(Input::RawMouseMoveInfo& info)
		{
			std::cout << "Raw mouse move ("
				<< "rawX=" << info.GetRawX() << ", "
				<< "rawY=" << info.GetRawY()
				<< ")"
				<< std::endl;
		}

		void OnKey(Input::KeyInfo& info)
		{
			std::string tru = "true";
			std::string fals = "false";

			std::cout << "Key (pressed="
				<< (info.IsPressed() ? tru : fals)
				<< ", scancode=" << info.GetScancode()
				<< ", key=" << info.GetKey()
				<< ")"
				<< std::endl;
		}

		void OnKeyChar(Input::KeyCharInfo& info)
		{
			std::string tru = "true";
			std::string fals = "false";

			std::cout << "Key char (repeat="
				<< (info.IsAutoRepeat() ? tru : fals)
				<< ", key=" << info.GetKey()
				<< ")"
				<< std::endl;
		}
	};

	TestWindow()
		:
		Window(1280, 720, "sup", false),
		handler(this)
	{
		AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		AddEventHandler(handler, Events::EventType::WINDOW_ERROR);
		AddEventHandler(handler, Events::EventType::WINDOW_RESIZE);
		AddEventHandler(handler, Events::EventType::WINDOW_MOVE);

		AddInputListener(listener, Input::InputType::MOUSE_MOVE);
		AddInputListener(listener, Input::InputType::RAW_MOUSE_MOVE);
		AddInputListener(listener, Input::InputType::KEY);
		AddInputListener(listener, Input::InputType::KEY_CHAR);

		SetRawInputEnabled(true);

		SetX(120);
		SetY(120);

		SetVisible(true);
	}

	~TestWindow()
	{
		RemoveEventHandler(handler);
	}
private:
	EventHandler handler;
	TestListener listener;
};

int main()
{
	TestWindow window;
	TestWindow window2;

	while (!window.IsCloseRequested() || !window2.IsCloseRequested())
	{
		window.PollEvents();
		window2.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	return 0;
}