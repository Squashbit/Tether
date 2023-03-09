#pragma once

#include <Tether/Application.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

namespace Tether::Platform
{
    class X11Application : public Application
    {
    public:
        X11Application();
        ~X11Application();

        struct Library
        {
            void* handle;
        };

        typedef int (*PFN_XISelectEvents)(Display*,XID,XIEventMask*,int);
        
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

        size_t GetMonitorCount() override;
        std::vector<Devices::Monitor> GetMonitors() override;
        
        const XILibrary& GetXI() const;
        const int GetScreen() const;
        Display* const GetDisplay() const;
        const unsigned int GetRoot() const;
        const Cursor GetHiddenCursor() const;
    private:
        void LoadLibraries();
		void LoadFunctions();
        void FreeLibraries();

        void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes) override;

        Keycodes TranslateScancode(const KeySym* keysyms, int width);

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
    ((X11Application&)Application::Get()).GetXI().SelectEvents(display, \
        window, masks, numMasks)
