#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class FullscreenWindow
{
public:
	FullscreenWindow()
		:
		m_Window(Window::Create(1280, 720, L"Fullscreen"))
	{
		m_Window->SetRawInputEnabled(true);

		m_Window->SetX(120);
		m_Window->SetY(120);

		m_Window->SetVisible(true);

		Devices::Monitor monitor = Application::Get().GetMonitors()[0];
		m_Window->SetFullscreen(true, monitor);

		m_Window->Run();
	}

	~FullscreenWindow()
	{
		m_Window->RemoveEventHandler(handler);
	}
private:
	Scope<Window> m_Window;
};

int main()
{
	FullscreenWindow window;

	return 0;
}