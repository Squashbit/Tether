#pragma once
#ifdef __linux__

#include <Tether/Native/SimpleWindowNative.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

namespace Tether::Native
{
	class TETHER_EXPORT X11SimpleWindow : public SimpleWindowNative
	{
	public:
	private:
		void ProcessMwmFunctions();
	private:
		uint64_t prevX = 0, prevY = 0;
		uint64_t prevWidth = 0, prevHeight = 0;

		unsigned long window = 0;
		int screen = 0;

		XEvent event;

		std::unordered_map<uint32_t, Time> pressTimes;
		uint32_t lastPressed = UINT32_MAX;
	};
}

#endif //__linux__