#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class TestWindow
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		void OnWindowResize(Events::WindowResizeEvent& event)
		{
			std::cout << "Resized window to W=" << event.GetNewWidth()
				<< ", H=" << event.GetNewHeight() << std::endl;
		}

		void OnWindowMove(Events::WindowMoveEvent& event)
		{
			std::cout << "Moved window to X=" << event.GetX()
				<< ", Y=" << event.GetY() << std::endl;
		}
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
		m_Window(Window::Create(1280, 720, L"sup"))
	{
		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_RESIZE);
		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_MOVE);

		m_Window->AddInputListener(listener, Input::InputType::MOUSE_MOVE);
		m_Window->AddInputListener(listener, Input::InputType::RAW_MOUSE_MOVE);
		m_Window->AddInputListener(listener, Input::InputType::KEY);
		m_Window->AddInputListener(listener, Input::InputType::KEY_CHAR);

		m_Window->SetRawInputEnabled(true);

		m_Window->SetX(120);
		m_Window->SetY(120);

		m_Window->SetVisible(true);
	}

	~TestWindow()
	{
		m_Window->RemoveEventHandler(handler);
	}

	void PollEvents()
	{
		m_Window->PollEvents();
	}

	bool IsCloseRequested()
	{
		return m_Window->IsCloseRequested();
	}
private:
	Scope<Window> m_Window;

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