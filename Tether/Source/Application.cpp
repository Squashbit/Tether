#include <Tether/Application.hpp>

#include <stdexcept>
#include <cstring>

#include <Tether/Platform/PlatformDefs.hpp>
#if defined(TETHER_PLATFORM_WINDOWS)
#include <Tether/Platform/Win32Application.hpp>
#elif defined(TETHER_PLATFORM_LINUX)
#include <Tether/Platform/X11Application.hpp>
#endif

namespace Tether
{
	Application::Application()
	{
		memset(m_Keycodes, -1, sizeof(m_Keycodes));
		memset(m_Scancodes, -1, sizeof(m_Scancodes));
	}

	Application::~Application() 
	{}

	const int16_t* const Application::GetKeycodes() const
	{
		return m_Keycodes;
	}

	const int16_t* const Application::GetScancodes() const
	{
		return m_Scancodes;
	}

	Application& Application::Get()
	{
		if (!internal.get())
		{
#if defined(TETHER_PLATFORM_WINDOWS)
			internal = std::make_unique<Platform::Win32Application>();
#elif defined(TETHER_PLATFORM_LINUX)
			internal = std::make_unique<Platform::X11Application>();
#endif

			internal->CreateKeyLUTs(internal->m_Keycodes, internal->m_Scancodes);
		}

		return *internal;
	}
}
