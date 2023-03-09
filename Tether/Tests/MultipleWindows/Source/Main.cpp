#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

static constexpr const size_t WINDOW_COUNT = 30;
static bool s_WindowsClosed[WINDOW_COUNT]{};

class TestWindow
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(Window& window, size_t windowIndex)
			:
			m_Window(window),
			m_WindowIndex(windowIndex)
		{
			if (windowIndex >= WINDOW_COUNT)
				throw std::invalid_argument("Window index larger than window count");
		}

		void OnWindowClosing() override;
		
		Window& m_Window;
		size_t m_WindowIndex = 0;
	};

	TestWindow(const Devices::Monitor& monitor, size_t windowIndex)
		:
		m_Window(Window::Create(700, 700, std::to_wstring(windowIndex + 1))),
		handler(*m_Window, windowIndex)
	{
		int usableWidth = monitor.GetWidth() - m_Window->GetWidth();
		int usableHeight = monitor.GetHeight() - m_Window->GetHeight();

		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);

		m_Window->SetResizable(false);
		m_Window->SetRawInputEnabled(true);

		m_Window->SetX(120);
		m_Window->SetY(120);

		m_Window->SetVisible(true);

		m_Window->SetX(rand() % std::max(0, usableWidth));
		m_Window->SetY(rand() % std::max(0, usableHeight));
	}

	TestWindow(TestWindow&& other) noexcept
		:
		m_Window(std::move(other.m_Window)),
		handler(*m_Window, other.handler.m_WindowIndex)
	{
		m_Window->RemoveEventHandler(other.handler);
		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
	}

	bool IsCloseRequested()
	{
		return m_Window->IsCloseRequested();
	}
private:
	Scope<Window> m_Window;

	EventHandler handler;
};

static std::vector<TestWindow> windows;

void TestWindow::EventHandler::OnWindowClosing()
{
	m_Window.SetVisible(false);
	s_WindowsClosed[m_WindowIndex] = true;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
		if (!s_WindowsClosed[i])
			return;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
	{
		windows[i].m_Window->SetVisible(true);
		s_WindowsClosed[i] = false;

		std::this_thread::sleep_for(15ms);
	}
}

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	windows.reserve(WINDOW_COUNT);

	Devices::Monitor monitor = Application::Get().GetMonitors()[0];

	for (size_t i = 0; i < WINDOW_COUNT; i++)
	{
		windows.push_back(TestWindow(monitor, i));
		std::this_thread::sleep_for(100ms);
	}

	Application::Get().Run();

	return 0;
}