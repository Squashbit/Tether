#pragma once

#include <Tether/Application.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

namespace Tether
{
    class X11Application : public Application
    {
    public:
        struct Library
        {
            void* handle;
        };

        typedef int (*PFN_XISelectEvents)(Display*,Window,XIEventMask*,int);
        
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
            PFN_XISelectEvents SelectEvents;
        };

        XRRLibrary xrr;
		XILibrary xi;
		
		Display* display;
		unsigned int root;
		int screen;

		Pixmap hiddenCursorPixmap;
        Cursor hiddenCursor;
    };
}

#define XISelectEvents(display, window, masks, numMasks) \
    ((X11Application&)Application::Get()).xi.SelectEvents(display, window, masks, numMasks)