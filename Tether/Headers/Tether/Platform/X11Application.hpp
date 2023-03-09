#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Library.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

namespace Tether::Platform
{
    class X11Application : public Application
    {
    public:
        struct XRRLibrary : public Library
        {
            XRRLibrary(Display* display);

            bool available = true;
            int opcode = 0;

            void* GetCrtcInfo = nullptr;
            void* GetMonitors = nullptr;
            void* GetOutputInfo = nullptr;
            void* GetScreenResources = nullptr;
            void* FreeCrtcInfo = nullptr;
            void* FreeMonitors = nullptr;
            void* FreeOutputInfo = nullptr;
            void* FreeScreenResources = nullptr;
        };

        struct XILibrary : public Library
        {
            XILibrary(Display* display);
            
            bool available = true;
            int opcode = 0;

            void* SelectEvents = nullptr;
        };

        X11Application();
        ~X11Application();

        void Run() override;
		void PollEvents() override;

        size_t GetMonitorCount() override;
        std::vector<Devices::Monitor> GetMonitors() override;
        
        const XRRLibrary& GetXRR() const;
        const XILibrary& GetXI() const;
        const int GetScreen() const;
        Display* const GetDisplay() const;
        const unsigned int GetRoot() const;
        const Cursor GetHiddenCursor() const;
        const XContext GetUserDataContext() const;
    private:

        void RedirectEventToWindow(XEvent& event);

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
