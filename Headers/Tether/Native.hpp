// This file includes window utilities that are platform specific.

#ifndef _TETHER_STORAGE_HPP
#define _TETHER_STORAGE_HPP

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
	struct VarStorage
	{
		HWND window = 0;
		// There is only ever one hinstance but for simplicity it is stored in the
		// window.
		HINSTANCE hinst = nullptr;

		WNDCLASSEX wndClass;
	};

	struct AppVarStorage
	{

	};
#elif __linux__
	struct VarStorage
	{
		unsigned long window = 0;
		int screen = 0;

		XEvent event;

		std::unordered_map<uint32_t, Time> pressTimes;
		uint32_t lastPressed = UINT32_MAX;
	};

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

#endif //_TETHER_STORAGE_HPP