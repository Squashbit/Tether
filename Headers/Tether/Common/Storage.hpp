// This file includes storage classes that are platform specific.
// Storage classes exist so that the windowing library header isn't included in the
// IWindow header. This file isn't meant to be included in a header.

#ifndef _TETHER_STORAGE_HPP
#define _TETHER_STORAGE_HPP

#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32

namespace Tether::Storage
{
#ifdef _WIN32
	struct WindowsVarStorage
	{
		HWND window = 0;
		// There is only ever one hinstance but for simplicity it is stored in the
		// window.
		HINSTANCE hinst = nullptr;

		WNDCLASSEX wndClass;
	};
#endif //_WIN32
}

#endif //_TETHER_STORAGE_HPP