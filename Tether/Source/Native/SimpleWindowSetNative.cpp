#include <Tether/SimpleWindow.hpp>
#include <Tether/Native/Win32SimpleWindow.hpp>
#include <Tether/Native/X11SimpleWindow.hpp>

void Tether::SimpleWindow::SetNative()
{
#ifdef _WIN32
	native = new Native::Win32SimpleWindow(this);
#endif

#ifdef __linux__
	native = new Native::X11SimpleWindow(this);
#endif
}