// This file includes window utilities that are platform specific.

#pragma once

#include <stdint.h>
#include <unordered_map>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>
#endif //__linux__

#include <Tether/Application.hpp>

namespace Tether::Storage
{
	struct Library
	{
		void* handle;
	};

#ifdef _WIN32
	struct AppVarStorage
	{
		
	};
#elif __linux__
	typedef int (*PFN_XISelectEvents)(Display*,Window,XIEventMask*,int);
	#define XISelectEvents(display, window, masks, numMasks) \
		Application::Get().storage->xi.selectEvents(display, window, masks, numMasks)
	
	struct XExtension
	{
		bool available = true;
	};
	
	struct XRRLibrary : public Library, public XExtension
	{
		
	};

	struct XILibrary : public Library, public XExtension
	{
		int opcode;
		PFN_XISelectEvents selectEvents;
	};

	struct AppVarStorage
	{
		XRRLibrary xrr;
		XILibrary xi;
		
		Display* display;
		unsigned int root;
		int screen;

		Pixmap hiddenCursorPixmap;
        Cursor hiddenCursor;

		int16_t keycodes[256];
		int16_t scancodes[256];
	};
#endif
}
