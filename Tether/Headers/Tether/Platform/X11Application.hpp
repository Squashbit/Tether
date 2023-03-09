#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Library.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>

namespace Tether::Platform
{
    class X11Application : public Application
    {
    public:
        typedef int (*PFN_XISelectEvents)(Display*,XID,XIEventMask*,int);
        
        struct XRRLibrary : public Library
        {
            XRRLibrary();

            bool available = true;
        };

        struct XILibrary : public Library
        {
            XILibrary(Display* display);
            
            bool available = true;
            int opcode;

            PFN_XISelectEvents SelectEvents;
        };

        X11Application();
        ~X11Application();

        void Run() override;
		void PollEvents() override;

        size_t GetMonitorCount() override;
        std::vector<Devices::Monitor> GetMonitors() override;
        
        const XILibrary& GetXI() const;
        const int GetScreen() const;
        Display* const GetDisplay() const;
        const unsigned int GetRoot() const;
        const Cursor GetHiddenCursor() const;
        const XContext GetUserDataContext() const;
    private:
        struct MonitorDisplayModes
        {
            std::optional<Devices::Monitor::DisplayMode> currentMode;
            std::vector<Devices::Monitor::DisplayMode> displayModes;
        };

        void RedirectEventToWindow(XEvent& event);

        void QueryDisplayModes(XRRScreenResources* resources,
            const XRRMonitorInfo& monitorInfo, MonitorDisplayModes& modes);

        void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes) override;

        Keycodes TranslateScancode(const KeySym* keysyms, int width);

        XContext m_UserDataContext;
		
		Display* display;
		unsigned int root;
		int screen;

        XRRLibrary xrr;
		XILibrary xi;

		Pixmap hiddenCursorPixmap;
        Cursor hiddenCursor;
    };
}

#define XISelectEvents(display, window, masks, numMasks) \
    ((X11Application&)Application::Get()).GetXI().SelectEvents(display, \
        window, masks, numMasks)
