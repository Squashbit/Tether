#include <Tether/Application.hpp>
#include <stdexcept>

#include <Tether/Platform/PlatformDefs.hpp>
#if defined(TETHER_PLATFORM_WINDOWS)
#include <Tether/Platform/Win32Application.hpp>
#elif defined(TETHER_PLATFORM_LINUX)
#include <Tether/Platform/X11Application.hpp>
#endif

namespace Tether
{
	const int16_t* const Application::GetKeycodes() const
	{
		return keycodes;
	}

	const int16_t* const Application::GetScancodes() const
	{
		return scancodes;
	}

	Application& Application::Get()
	{
		if (!internal.get())
		{
#if defined(TETHER_PLATFORM_WINDOWS)
			internal = std::make_unique<Win32Application>();
#elif defined(TETHER_PLATFORM_LINUX)
			internal = std::make_unique<X11Application>();
#endif
		}

		return *internal;
	}
}
