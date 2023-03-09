#include <Tether/Platform/X11Window.hpp>
#include <Tether/Platform/X11Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

struct MwmHints 
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
};

enum 
{
    MWM_HINTS_FUNCTIONS = (1L << 0),
    MWM_HINTS_DECORATIONS =  (1L << 1),

    MWM_FUNC_ALL = (1L << 0),
    MWM_FUNC_RESIZE = (1L << 1),
    MWM_FUNC_MOVE = (1L << 2),
    MWM_FUNC_MINIMIZE = (1L << 3),
    MWM_FUNC_MAXIMIZE = (1L << 4),
    MWM_FUNC_CLOSE = (1L << 5)
};

namespace Tether::Platform
{
    X11Window::X11Window(int width, int height, std::wstring_view title, 
		bool visible)
        :
        m_App((X11Application&)Application::Get())
    {
        unsigned long root = RootWindow(m_App.GetDisplay(), 
            m_App.GetScreen());

        XWindowAttributes attrs;
        XGetWindowAttributes(m_App.GetDisplay(), root, &attrs);
        
        XSetWindowAttributes swa{};
        swa.event_mask = 
            PointerMotionMask 
            | ButtonPressMask
            | ButtonReleaseMask
            | StructureNotifyMask
            | KeyPressMask
            | KeyReleaseMask;

        m_Window = XCreateWindow(
            m_App.GetDisplay(), 
            root,
            attrs.x, attrs.y,
            width, height,
            0, // Border width
            DefaultDepth(m_App.GetDisplay(), m_App.GetScreen()),
            InputOutput,
            DefaultVisual(m_App.GetDisplay(), m_App.GetScreen()),
            CWEventMask,
            &swa
        );
        
        Atom wmDelete = XInternAtom(m_App.GetDisplay(), "WM_DELETE_WINDOW", true);
        XSetWMProtocols(m_App.GetDisplay(), m_Window, &wmDelete, 1);
        
        XSaveContext(m_App.GetDisplay(), m_Window, 
            m_App.GetUserDataContext(), (XPointer)this);

        SetVisible(visible);
        
        XStoreName(m_App.GetDisplay(), m_Window, 
            m_WideConverter.to_bytes(title.data()).c_str());

        XFlush(m_App.GetDisplay());
    }

    X11Window::~X11Window()
    {
        XLockDisplay(m_App.GetDisplay());
            XDeleteContext(m_App.GetDisplay(), m_Window, 
                m_App.GetUserDataContext());
            XUnmapWindow(m_App.GetDisplay(), m_Window);
            XDestroyWindow(m_App.GetDisplay(), m_Window);
            XFlush(m_App.GetDisplay());
        XUnlockDisplay(m_App.GetDisplay());
    }

    void X11Window::SetVisible(bool visibility)
    {
        if (visibility)
            XMapWindow(m_App.GetDisplay(), m_Window);
        else
            XUnmapWindow(m_App.GetDisplay(), m_Window);
        
        m_Visible = visibility;
    }

    bool X11Window::IsVisible()
    {
        return m_Visible;
    }

    void X11Window::SetX(int x)
    {
        XMoveWindow(m_App.GetDisplay(), m_Window, x, GetY());
    }

    void X11Window::SetY(int y)
    {
        XMoveWindow(m_App.GetDisplay(), m_Window, GetX(), y);
    }

    void X11Window::SetPosition(int x, int y)
    {
        XMoveWindow(m_App.GetDisplay(), m_Window, x, y);
    }

    void X11Window::SetWidth(int width)
    {
        XResizeWindow(m_App.GetDisplay(), m_Window, width, GetHeight());
    }

    void X11Window::SetHeight(int height)
    {
        XResizeWindow(m_App.GetDisplay(), m_Window, GetWidth(), height);
    }

    void X11Window::SetSize(int width, int height)
    {
        XResizeWindow(m_App.GetDisplay(), m_Window, width, height);
    }

    void X11Window::SetTitle(std::wstring_view title)
    {
        XStoreName(m_App.GetDisplay(), m_Window, 
            m_WideConverter.to_bytes(title.data()).c_str());
    }

    void X11Window::SetBoundsEnabled(bool enabled)
    {
        if (m_BoundsEnabled == enabled)
            return;
        m_BoundsEnabled = enabled;

        if (enabled)
            SetBounds(m_MinWidth, m_MinHeight, m_MaxWidth, m_MaxHeight);
        else
            SetBounds(INT32_MIN, INT32_MIN, INT32_MAX, INT32_MAX);
    }

    void X11Window::SetBounds(int minWidth, int minHeight, 
        int maxWidth, int maxHeight)
    {
        XSizeHints sizeHints{};
        sizeHints.min_width = minWidth;
        sizeHints.min_height = minHeight;
        sizeHints.max_width = maxWidth;
        sizeHints.max_height = maxHeight;
        sizeHints.flags = PMinSize | PMaxSize;

        m_MinWidth  = minWidth;
        m_MinHeight = minHeight;
        m_MaxWidth  = maxWidth;
        m_MaxHeight = maxHeight;

        XSetWMSizeHints(m_App.GetDisplay(), m_Window, &sizeHints, 
            XA_WM_NORMAL_HINTS);
    }

    void X11Window::SetDecorated(bool decorated)
    {
        Atom motifWmHints = XInternAtom(m_App.GetDisplay(), "_MOTIF_WM_HINTS", true);

        MwmHints hints{};
        hints.flags = MWM_HINTS_DECORATIONS;
        hints.decorations = decorated;

        XChangeProperty(
            m_App.GetDisplay(), m_Window,
            motifWmHints,
            motifWmHints, 
            32,
            PropModeReplace,
            (unsigned char*)&hints,
            sizeof(hints) / sizeof(long)
        );
    }

    void X11Window::SetResizable(bool resizable)
    {
        m_Resizable = resizable;
        ProcessMwmFunctions();
    }

    void X11Window::SetClosable(bool closable)
    {
        m_Closable = closable;
        ProcessMwmFunctions();
    }

    void X11Window::SetButtonStyleBitmask(uint8_t mask)
    {
        m_StyleMask = mask;
        ProcessMwmFunctions();
    }

    void X11Window::SetMaximized(bool maximized)
    {
        Atom wmState = XInternAtom(m_App.GetDisplay(), "_NET_WM_STATE", true);
        Atom maxHorzAtom = XInternAtom(m_App.GetDisplay(), 
            "_NET_WM_STATE_MAXIMIZED_HORZ", true);
        Atom maxVertAtom = XInternAtom(m_App.GetDisplay(), 
            "_NET_WM_STATE_MAXIMIZED_VERT", true);
        if (maximized)
        {
            XEvent fullscreenEvent{};
            fullscreenEvent.type = ClientMessage;
            fullscreenEvent.xclient.window = m_Window;
            fullscreenEvent.xclient.message_type = wmState;
            fullscreenEvent.xclient.format = 32;
            fullscreenEvent.xclient.data.l[0] = 1; // Add
            fullscreenEvent.xclient.data.l[1] = maxHorzAtom;
            fullscreenEvent.xclient.data.l[2] = maxVertAtom;
            
            XSendEvent(m_App.GetDisplay(), DefaultRootWindow(m_App.GetDisplay()), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
        }
        else
        {
            XEvent fullscreenEvent{};
            fullscreenEvent.type = ClientMessage;
            fullscreenEvent.xclient.window = m_Window;
            fullscreenEvent.xclient.message_type = wmState;
            fullscreenEvent.xclient.format = 32;
            fullscreenEvent.xclient.data.l[0] = 0; // Remove
            fullscreenEvent.xclient.data.l[1] = maxHorzAtom;
            fullscreenEvent.xclient.data.l[2] = maxVertAtom;
            
            XSendEvent(m_App.GetDisplay(), DefaultRootWindow(m_App.GetDisplay()), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
        }

        XSync(m_App.GetDisplay(), false);
    }

    void X11Window::SetPreferredResizeInc(int x, int y)
    {
        XSizeHints sizeHints{};
        sizeHints.width_inc = x;
        sizeHints.height_inc = y;
        sizeHints.flags = PResizeInc;

        XSetWMSizeHints(m_App.GetDisplay(), m_Window, &sizeHints, 
            XA_WM_NORMAL_HINTS);
    }

    void X11Window::SetFullscreen(bool fullscreen, 
        const Devices::Monitor& monitor)
    {
        if (m_Fullscreen == fullscreen)
            return;

        Atom wmState = XInternAtom(m_App.GetDisplay(), "_NET_WM_STATE", true);
        Atom fullscreenAtom = XInternAtom(m_App.GetDisplay(), 
            "_NET_WM_STATE_FULLSCREEN", true);
        if (fullscreen)
        {
            XEvent fullscreenEvent{};
            fullscreenEvent.type = ClientMessage;
            fullscreenEvent.xclient.window = m_Window;
            fullscreenEvent.xclient.message_type = wmState;
            fullscreenEvent.xclient.format = 32;
            fullscreenEvent.xclient.data.l[0] = 2; // Replace
            fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
            
            XSendEvent(m_App.GetDisplay(), DefaultRootWindow(m_App.GetDisplay()), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
            
            XSync(m_App.GetDisplay(), false);
            
            Atom wmFullscreenMonitors = 
                XInternAtom(m_App.GetDisplay(), "_NET_WM_FULLSCREEN_MONITORS", true);
            XEvent event{};
            event.type = ClientMessage;
            event.xclient.window = m_Window;
            event.xclient.message_type = wmFullscreenMonitors;
            event.xclient.format = 32;

            event.xclient.data.l[0] = monitor.GetIndex();
            event.xclient.data.l[1] = monitor.GetIndex();
            event.xclient.data.l[2] = monitor.GetIndex();
            event.xclient.data.l[3] = monitor.GetIndex();

            XSendEvent(m_App.GetDisplay(), DefaultRootWindow(m_App.GetDisplay()), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, &event);
        }
        else
        {
            XEvent fullscreenEvent{};
            fullscreenEvent.type = ClientMessage;
            fullscreenEvent.xclient.window = m_Window;
            fullscreenEvent.xclient.message_type = wmState;
            fullscreenEvent.xclient.format = 32;
            fullscreenEvent.xclient.data.l[0] = 0; // Remove
            fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
            
            XSendEvent(m_App.GetDisplay(), DefaultRootWindow(m_App.GetDisplay()), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
        }

        XSync(m_App.GetDisplay(), false);
        
        m_Fullscreen = fullscreen;
    }

    void X11Window::SetRawInputEnabled(bool enabled)
    {
        if (!m_App.GetXI().GetHandle() || !m_App.GetXI().available)
            throw std::runtime_error("XI library not loaded");

        if (m_RawInputEnabled == enabled)
            return;
        
        if (enabled)
        {
            XIEventMask eventMask;
            unsigned char mask[XIMaskLen(XI_RawMotion)] = { 0 };

            eventMask.deviceid = XIAllMasterDevices;
            eventMask.mask_len = sizeof(mask);
            eventMask.mask = mask;
            XISetMask(mask, XI_RawMotion);

            XISelectEvents(m_App.GetDisplay(), m_App.GetRoot(), 
                &eventMask, 1);
        }
        else
        {
            XIEventMask eventMask;
            unsigned char mask[] = { 0 };

            eventMask.deviceid = XIAllMasterDevices;
            eventMask.mask_len = sizeof(mask);
            eventMask.mask = mask;

            XISelectEvents(m_App.GetDisplay(), m_App.GetRoot(), 
                &eventMask, 1);
        }

        m_RawInputEnabled = enabled;
    }

    void X11Window::SetCursorMode(Window::CursorMode mode)
    {
        switch (mode)
        {
            case Window::CursorMode::NORMAL:
            {
                XUngrabPointer(m_App.GetDisplay(), CurrentTime);
                XUndefineCursor(m_App.GetDisplay(), m_Window);
            }
            break;

            case Window::CursorMode::HIDDEN:
            {
                XDefineCursor(m_App.GetDisplay(), m_Window, 
                    m_App.GetHiddenCursor());
            }
            break;

            case Window::CursorMode::DISABLED:
            {
                XGrabPointer(
                    m_App.GetDisplay(), m_App.GetRoot(), true,
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                    GrabModeAsync, GrabModeAsync, 
                    m_App.GetRoot(), m_App.GetHiddenCursor(), 
                    CurrentTime
                );
            }
            break;
        }
    }

    void X11Window::SetCursorPos(int x, int y)
    {
        XWarpPointer(m_App.GetDisplay(), m_Window, m_Window, 0, 0, 
            0, 0, x, y);
    }

    void X11Window::SetCursorRootPos(int x, int y)
    {
        unsigned int root = DefaultRootWindow(m_App.GetDisplay());
        XWarpPointer(m_App.GetDisplay(), root, root, 0, 0, 0, 0, x, y);
    }

    int X11Window::GetX()
    {
        long unsigned int child;

        int x, y;
        XTranslateCoordinates(m_App.GetDisplay(), m_Window, 
            DefaultRootWindow(m_App.GetDisplay()), 0, 0, &x, &y, &child);
        
        return x;
    }

    int X11Window::GetY()
    {
        long unsigned int child;

        int x, y;
        XTranslateCoordinates(m_App.GetDisplay(), m_Window, 
            DefaultRootWindow(m_App.GetDisplay()), 0, 0, &x, &y, &child);
        
        return y;
    }

    int X11Window::GetWidth()
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(m_App.GetDisplay(), m_Window, &attribs);

        return attribs.width;
    }

    int X11Window::GetHeight()
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(m_App.GetDisplay(), m_Window, &attribs);

        return attribs.height;
    }

    int X11Window::GetMouseX()
    {
        return m_MouseX;
    }

    int X11Window::GetMouseY()
    {
        return m_MouseY;
    }

    int X11Window::GetRelativeMouseX()
    {
        return m_RelMouseX;
    }

    int X11Window::GetRelativeMouseY()
    {
        return m_RelMouseY;
    }

    bool X11Window::IsFocused()
    {
        XID focusedWindow;
        int revertTo;

        XGetInputFocus(m_App.GetDisplay(), &focusedWindow, &revertTo);

        return focusedWindow == m_Window;
    }

    unsigned long X11Window::GetWindowHandle() const
    {
        return m_Window;
    }

    void X11Window::ProcessEvent(XEvent& event)
    {
        using namespace Events;
        using namespace Input;
        
        switch (event.type)
        {
            case GenericEvent:
            {
                if (!m_RawInputEnabled)
                    return;

                XGenericEventCookie* cookie = &event.xcookie;
                if (!XGetEventData(m_App.GetDisplay(), cookie))
                    return;

                if (cookie->extension != m_App.GetXI().opcode
                    || cookie->evtype != XI_RawMotion)
                    return;
                
                XIRawEvent* data = (XIRawEvent*)cookie->data;
                const double* values = data->raw_values;

                uint64_t x = 0;
                uint64_t y = 0;

                if (XIMaskIsSet(data->valuators.mask, 0))
                {
                    x = *values;
                    values++;
                }

                if (XIMaskIsSet(data->valuators.mask, 1))
                    y = *values;
                
                if (x == 0 && y == 0)
                    return;

                RawMouseMoveInfo moveInfo(
                    x,
                    y
                );

                SpawnInput(InputType::RAW_MOUSE_MOVE, 
                [&](InputListener& inputListener)
                {
                    inputListener.OnRawMouseMove(moveInfo);
                });

                XFreeEventData(m_App.GetDisplay(), cookie);
            }
            break;

            case KeyPress:
            {
                Time eventTime = event.xkey.time;
                const uint32_t scancode = event.xkey.keycode;

                // Since Xlib doesn't have a built in way of telling if a 
                // KeyPress event was a repeat or not, 
                // this is the best I can manage.
                
                bool repeat = scancode == m_LastPressed;
                if (!repeat)
                {
                    KeyInfo keyInfo(
                        scancode,
                        m_App.GetKeycodes()[scancode],
                        true
                    );

                    SpawnInput(InputType::KEY, 
                    [&](InputListener& inputListener)
                    {
                        inputListener.OnKey(keyInfo);
                    });

                    m_LastPressed = scancode;
                }
                
                KeyCharInfo keyCharInfo(
                    m_App.GetKeycodes()[scancode],
                    repeat
                );

                SpawnInput(InputType::KEY_CHAR, 
                [&](InputListener& inputListener)
                {
                    inputListener.OnKeyChar(keyCharInfo);
                });
            }
            break;

            case KeyRelease:
            {
                const uint32_t scancode = event.xkey.keycode;

                if (XEventsQueued(m_App.GetDisplay(), QueuedAfterReading))
                {
                    XEvent nextEvent;
                    XPeekEvent(m_App.GetDisplay(), &nextEvent);

                    // The last check is if the time of the key pressed is 
                    // within 20 milliseconds of each other (the interval that
                    // XIM sends out key press repeats)
                    if (nextEvent.type == KeyPress &&
                        nextEvent.xkey.window == event.xkey.window &&
                        nextEvent.xkey.keycode == scancode &&
                        (nextEvent.xkey.time - event.xkey.time) < 20)
                        return;
                }

                KeyInfo keyInfo(
                    scancode,
                    m_App.GetKeycodes()[scancode],
                    false
                );

                SpawnInput(InputType::KEY, 
                [&](InputListener& inputListener)
                {
                    inputListener.OnKey(keyInfo);
                });

                m_LastPressed = UINT32_MAX;
            }
            break;
            
            case MotionNotify:
            {
                if (!m_PrevReceivedMouseMove)
                {
                    m_MouseX = event.xmotion.x_root;
                    m_MouseY = event.xmotion.y_root;
                    m_RelMouseX = event.xmotion.x;
                    m_RelMouseY = event.xmotion.y;
                }

                MouseMoveInfo mouseMove(
                    event.xmotion.x_root,
                    event.xmotion.y_root,
                    event.xmotion.x,
                    event.xmotion.y,
                    m_RelMouseX,
                    m_RelMouseY,
                    m_MouseX,
                    m_MouseY
                );
                
                SpawnInput(InputType::MOUSE_MOVE, 
                [&](InputListener& inputListener)
                {
                    inputListener.OnMouseMove(mouseMove);
                });

                m_MouseX = event.xmotion.x_root;
                m_MouseY = event.xmotion.y_root;
                m_RelMouseX = event.xmotion.x;
                m_RelMouseY = event.xmotion.y;

                m_PrevReceivedMouseMove = true;
            }
            break;

            case ConfigureNotify:
            {
                XConfigureEvent xce = event.xconfigure;

                // Verify that the event was a move event
                if (xce.width != m_PrevWidth || xce.height != m_PrevHeight)
                {
                    WindowResizeEvent linkEvent(
                        xce.width,
                        xce.height
                    );

                    SpawnEvent(Events::EventType::WINDOW_RESIZE, 
                    [&](Events::EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowResize(linkEvent);
                    });

                    m_PrevWidth = xce.width;
                    m_PrevHeight = xce.height;

                    return;
                }

                if (xce.x != m_PrevX || xce.y != m_PrevY)
                {
                    WindowMoveEvent event(
                        xce.x,
                        xce.y
                    );

                    SpawnEvent(Events::EventType::WINDOW_MOVE,
                    [&](Events::EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowMove(event);
                    });

                    m_PrevX = xce.x;
                    m_PrevY = xce.y;
                }
            }
            break;

            case ClientMessage:
            {
                // Check for WM_PROTOCOLS
                if (event.xclient.message_type != 
                    XInternAtom(m_App.GetDisplay(), "WM_PROTOCOLS", false))
                    break;
                
                SetCloseRequested(true);
                
                SpawnEvent(Events::EventType::WINDOW_CLOSING, 
                [&](Events::EventHandler& eventHandler)
                {
                    eventHandler.OnWindowClosing();
                });
            }
            break;

            case ButtonPress:
            {
                DispatchMouseButton(event, true);
            }
            break;

            case ButtonRelease:
            {
                DispatchMouseButton(event, false);
            }
            break;
        }
    }

    void X11Window::DispatchMouseButton(XEvent& event, bool pressed)
    {
        using namespace Input;
        using ClickType = MouseClickInfo::ClickType;

        ClickType type = ClickType::LEFT_BUTTON;
        switch (event.xbutton.button)
        {
            case Button2: type = ClickType::MIDDLE_BUTTON; break;
            case Button3: type = ClickType::RIGHT_BUTTON; break;
            case Button4: type = ClickType::SIDE_BUTTON1; break;
            case Button5: type = ClickType::SIDE_BUTTON2; break;
        }

        Input::MouseClickInfo input(
            event.xbutton.x_root,
            event.xbutton.y_root,
            event.xbutton.x,
            event.xbutton.y,
            type,
            pressed
        );

        SpawnInput(InputType::MOUSE_CLICK,
        [&](InputListener& inputListener)
        {
            inputListener.OnMouseClick(input);
        });
    }

    void X11Window::ProcessMwmFunctions()
    {
        Atom motifWmHints = XInternAtom(m_App.GetDisplay(), "_MOTIF_WM_HINTS", true);

        MwmHints hints{};
        hints.flags = MWM_HINTS_FUNCTIONS;
        hints.functions = MWM_FUNC_MOVE;

        if (m_Closable)
            hints.functions |= MWM_FUNC_CLOSE;
        if (m_Resizable)
            hints.functions |= MWM_FUNC_RESIZE;
        if (m_StyleMask & ButtonStyleMask::MINIMIZE_BUTTON)
            hints.functions |= MWM_FUNC_MINIMIZE;
        if (m_StyleMask & ButtonStyleMask::MAXIMIZE_BUTTON)
            hints.functions |= MWM_FUNC_MAXIMIZE;
        
        XChangeProperty(
            m_App.GetDisplay(), m_Window,
            motifWmHints,
            motifWmHints, 
            32,
            PropModeReplace,
            (unsigned char*)&hints,
            sizeof(hints) / sizeof(long)
        );
    }

}