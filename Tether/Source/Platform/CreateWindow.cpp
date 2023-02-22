#include <Tether/Platform/PlatformDefs.hpp>

#if defined(TETHER_PLATFORM_WINDOWS)
#include <Tether/Platform/Win32Window.hpp>
#elif defined(TETHER_PLATFORM_LINUX)
#include <Tether/Platform/X11Window.hpp>
#endif

namespace Tether
{
	Scope<Window> Window::Create(int width, int height, std::wstring_view title,
		bool visible)
	{
#if defined(TETHER_PLATFORM_WINDOWS)
		return std::make_unique<Platform::Win32Window>(width, height, title, visible);
#elif defined(TETHER_PLATFORM_LINUX)
		return std::make_unique<Platform::X11Window>(width, height, title, visible);
#endif
	}
}
