#include <Tether/Platform/X11Window.hpp>
#include <Tether/Native.hpp>
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

#define xdisplay m_App->storage->display
#define xscreen m_App->storage->screen

namespace Tether::Platform
{
    X11Window::X11Window(int width, int height, std::wstring_view title, 
		bool visible)
    {
        m_App = &Application::Get();
        
        this->setWidth = width;
        this->setHeight = height;

        unsigned long root = RootWindow(xdisplay, xscreen);
        
        XSetWindowAttributes swa{};
        swa.event_mask = 
            PointerMotionMask 
            | ButtonPressMask
            | ButtonReleaseMask
            | StructureNotifyMask
            | KeyPressMask
            | KeyReleaseMask;

        m_Window = XCreateWindow(
            xdisplay, 
            root,
            setX, setY,
            width, height,
            0, // Border width
            DefaultDepth(xdisplay, xscreen),
            InputOutput,
            DefaultVisual(xdisplay, xscreen),
            CWEventMask,
            &swa
        );
        
        Atom wmDelete = XInternAtom(xdisplay, "WM_DELETE_WINDOW", true);
        XSetWMProtocols(xdisplay, m_Window, &wmDelete, 1);

        if (shouldShow)
        {
            XMapWindow(xdisplay, m_Window);
            visible = true;
        }
        
        XMoveWindow(xdisplay, m_Window, setX, setY);
        XStoreName(xdisplay, m_Window, title);

        XFlush(xdisplay);

        initialized = true;
        OnInit();

        return true;
    }

    X11Window::~X11Window()
    {
        XLockDisplay(xdisplay);
            XUnmapWindow(xdisplay, m_Window);
            XDestroyWindow(xdisplay, m_Window);
            XFlush(xdisplay);
        XUnlockDisplay(xdisplay);

        hints.clear();
    }

    void X11Window::SetVisible(bool visibility)
    {
        if (visibility)
            XMapWindow(xdisplay, m_Window);
        else
            XUnmapWindow(xdisplay, m_Window);
        
        visible = visibility;
    }

    bool X11Window::IsVisible()
    {
        return visible && initialized;
    }

    void X11Window::SetX(int x)
    {
        XMoveWindow(xdisplay, m_Window, x, GetY());
    }

    void X11Window::SetY(int y)
    {
        XMoveWindow(xdisplay, m_Window, GetX(), y);
    }

    void X11Window::SetPosition(int x, int y)
    {
        XMoveWindow(xdisplay, m_Window, x, y);
    }

    void X11Window::SetWidth(int width)
    {
        XResizeWindow(xdisplay, m_Window, width, GetHeight());
    }

    void X11Window::SetHeight(int height)
    {
        XResizeWindow(xdisplay, m_Window, GetWidth(), height);
    }

    void X11Window::SetSize(int width, int height)
    {
        XResizeWindow(xdisplay, m_Window, width, height);
    }

    void X11Window::SetTitle(const char* title)
    {
        XStoreName(xdisplay, m_Window, title);
    }

    void X11Window::SetBoundsEnabled(bool enabled)
    {
        if (this->boundsEnabled == enabled)
            return;
        this->boundsEnabled = enabled;

        if (enabled)
            SetBounds(minWidth, minHeight, maxWidth, maxHeight);
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

        this->minWidth  = minWidth;
        this->minHeight = minHeight;
        this->maxWidth  = maxWidth;
        this->maxHeight = maxHeight;

        XSetWMSizeHints(xdisplay, m_Window, &sizeHints, 
            XA_WM_NORMAL_HINTS);
    }

    void X11Window::SetDecorated(bool decorated)
    {
        Atom motifWmHints = XInternAtom(xdisplay, "_MOTIF_WM_HINTS", true);

        MwmHints hints{};
        hints.flags = MWM_HINTS_DECORATIONS;
        hints.decorations = decorated;

        XChangeProperty(
            xdisplay, m_Window,
            motifWmHints,
            motifWmHints, 
            32,
            PropModeReplace,
            (unsigned char*)&hints,
            sizeof(hints) / sizeof(long)
        );
    }

    void X11Window::SetClosable(bool closable)
    {
        if (this->closable == closable)
            return;
        this->closable = closable;

        ProcessMwmFunctions();
    }

    void X11Window::SetResizable(bool isResizable)
    {
        if (isResizable == resizable)
            return;
        resizable = isResizable;

        ProcessMwmFunctions();
    }

    void X11Window::SetMinimizeBox(bool enabled)
    {
        if (this->minimizeBox == enabled)
            return;
        this->minimizeBox = enabled;

        ProcessMwmFunctions();
    }

    void X11Window::SetMaximized(bool maximized)
    {
        Atom wmState = XInternAtom(xdisplay, "_NET_WM_STATE", true);
        Atom maxHorzAtom = XInternAtom(xdisplay, 
            "_NET_WM_STATE_MAXIMIZED_HORZ", true);
        Atom maxVertAtom = XInternAtom(xdisplay, 
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
            
            XSendEvent(xdisplay, DefaultRootWindow(xdisplay), 
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
            
            XSendEvent(xdisplay, DefaultRootWindow(xdisplay), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
        }

        XSync(xdisplay, false);
    }

    void X11Window::SetMaximizeBox(bool enabled)
    {
        if (this->maximizeBox == enabled)
            return;
        this->maximizeBox = enabled;

        ProcessMwmFunctions();
    }

    void X11Window::SetPreferredResizeInc(int x, int y)
    {
        XSizeHints sizeHints{};
        sizeHints.width_inc = x;
        sizeHints.height_inc = y;
        sizeHints.flags = PResizeInc;

        XSetWMSizeHints(xdisplay, m_Window, &sizeHints, 
            XA_WM_NORMAL_HINTS);
    }

    void X11Window::SetFullscreen(
        bool fullscreen, 
        const FullscreenSettings& settings,
		const Devices::Monitor& monitor
    )
    {
        if (this->fullscreen == fullscreen)
            return;

        Atom wmState = XInternAtom(xdisplay, "_NET_WM_STATE", true);
        Atom fullscreenAtom = XInternAtom(xdisplay, 
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
            
            XSendEvent(xdisplay, DefaultRootWindow(xdisplay), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
            
            XSync(xdisplay, false);
            
            Atom wmFullscreenMonitors = 
                XInternAtom(xdisplay, "_NET_WM_FULLSCREEN_MONITORS", true);
            XEvent event{};
            event.type = ClientMessage;
            event.xclient.window = m_Window;
            event.xclient.message_type = wmFullscreenMonitors;
            event.xclient.format = 32;

            if (monitor)
            {
                event.xclient.data.l[0] = monitor->index;
                event.xclient.data.l[1] = monitor->index;
                event.xclient.data.l[2] = monitor->index;
                event.xclient.data.l[3] = monitor->index;
            }

            XSendEvent(xdisplay, DefaultRootWindow(xdisplay), 
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
            
            XSendEvent(xdisplay, DefaultRootWindow(xdisplay), 
                false, 
                SubstructureRedirectMask | SubstructureNotifyMask, 
                &fullscreenEvent);
        }

        XSync(xdisplay, false);
        
        this->fullscreen = fullscreen;
    }

    void X11Window::SetRawInputEnabled(bool enabled)
    {
        if (!m_App->storage->xi.handle || !m_App->storage->xi.available)
        {
            DispatchError(ErrorCode::LIBRARY_NOT_LOADED, ErrorSeverity::HIGH, 
                "Window::SetRawInputEnabled");
            return;
        }

        if (enabled == rawInputEnabled)
            return;
        
        if (enabled)
        {
            XIEventMask eventMask;
            unsigned char mask[XIMaskLen(XI_RawMotion)] = { 0 };

            eventMask.deviceid = XIAllMasterDevices;
            eventMask.mask_len = sizeof(mask);
            eventMask.mask = mask;
            XISetMask(mask, XI_RawMotion);

            XISelectEvents(xdisplay, m_App->storage->root, &eventMask, 1);
        }
        else
        {
            XIEventMask eventMask;
            unsigned char mask[] = { 0 };

            eventMask.deviceid = XIAllMasterDevices;
            eventMask.mask_len = sizeof(mask);
            eventMask.mask = mask;

            XISelectEvents(xdisplay, m_App->storage->root, &eventMask, 1);
        }

        rawInputEnabled = enabled;
    }

    void X11Window::SetCursorMode(Window::CursorMode mode)
    {
        switch (mode)
        {
            case Window::CursorMode::NORMAL:
            {
                XUngrabPointer(xdisplay, CurrentTime);
                XUndefineCursor(xdisplay, m_Window);
            }
            break;

            case Window::CursorMode::HIDDEN:
            {
                XDefineCursor(xdisplay, m_Window, 
                    m_App->storage->hiddenCursor);
            }
            break;

            case Window::CursorMode::DISABLED:
            {
                XGrabPointer(
                    xdisplay, m_App->storage->root, true,
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                    GrabModeAsync, GrabModeAsync, 
                    m_App->storage->root, m_App->storage->hiddenCursor, 
                    CurrentTime
                );
            }
            break;
        }

        cursorMode = mode;
    }

    void X11Window::SetCursorPos(int x, int y)
    {
        XWarpPointer(xdisplay, m_Window, m_Window, 0, 0, 
            0, 0, x, y);
    }

    void X11Window::SetCursorRootPos(int x, int y)
    {
        unsigned int root = DefaultRootWindow(xdisplay);
        XWarpPointer(xdisplay, root, root, 0, 0, 0, 0, x, y);
    }

    int64_t X11Window::GetX()
    {
        long unsigned int child;

        int x, y;
        XTranslateCoordinates(xdisplay, m_Window, 
            DefaultRootWindow(xdisplay), 0, 0, &x, &y, &child);
        
        return x;
    }

    int64_t X11Window::GetY()
    {
        long unsigned int child;

        int x, y;
        XTranslateCoordinates(xdisplay, m_Window, 
            DefaultRootWindow(xdisplay), 0, 0, &x, &y, &child);
        
        return y;
    }

    uint64_t X11Window::GetWidth()
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(xdisplay, m_Window, &attribs);

        return attribs.width;
    }

    uint64_t X11Window::GetHeight()
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(xdisplay, m_Window, &attribs);

        return attribs.height;
    }

    void X11Window::PollEvents()
    {
        using namespace Events;
        using namespace Input;

        if (!visible)
            return;

        bool eventReceived = false;
        while (XPending(xdisplay))
        {
            XNextEvent(xdisplay, &storage->event);
            eventReceived = true;
            
            switch (storage->event.type)
            {
                case GenericEvent:
                {
                    if (!rawInputEnabled)
                        continue;

                    XGenericEventCookie* cookie = &storage->event.xcookie;
                    if (XGetEventData(xdisplay, cookie))
                    {
                        if (cookie->extension != m_App->storage->xi.opcode
                            || cookie->evtype != XI_RawMotion)
                            continue;
                        
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
                            continue;

                        Input::RawMouseMoveInfo moveInfo(
                            x,
                            y
                        );

                        SpawnInput(Input::InputType::RAW_MOUSE_MOVE, 
                        [&](Input::InputListener* pInputListener)
                        {
                            pInputListener->OnRawMouseMove(moveInfo);
                        });

                        XFreeEventData(xdisplay, cookie);
                        continue;
                    }
                }
                break;

                case KeyPress:
                {
                    Time eventTime = storage->event.xkey.time;
                    const uint32_t scancode = storage->event.xkey.keycode;

                    // Since Xlib doesn't have a built in way of telling if a 
                    // KeyPress event was a repeat or not, 
                    // this is the best I can manage.
                    
                    bool repeat = scancode == storage->lastPressed;
                    if (!repeat)
                    {
                        Input::KeyInfo keyInfo(
                            scancode,
                            m_App->storage->keycodes[scancode],
                            true
                        );

                        SpawnInput(Input::InputType::KEY, 
                        [&](Input::InputListener* pInputListener)
                        {
                            pInputListener->OnKey(keyInfo);
                        });

                        storage->lastPressed = scancode;
                    }
                    storage->pressTimes[scancode] = eventTime;

                    Input::KeyCharInfo keyCharInfo(
                        m_App->storage->keycodes[scancode],
                        repeat
                    );

                    SpawnInput(Input::InputType::KEY_CHAR, 
                    [&](Input::InputListener* pInputListener)
                    {
                        pInputListener->OnKeyChar(keyCharInfo);
                    });
                }
                break;

                case KeyRelease:
                {
                    const uint32_t scancode = storage->event.xkey.keycode;

                    if (XEventsQueued(xdisplay, QueuedAfterReading))
                    {
                        XEvent nextEvent;
                        XPeekEvent(xdisplay, &nextEvent);

                        // The last check is if the time of the key pressed is 
                        // within 20 milliseconds of each other (the interval that
                        // XIM sends out key press repeats)
                        if (nextEvent.type == KeyPress &&
                            nextEvent.xkey.window == storage->event.xkey.window &&
                            nextEvent.xkey.keycode == scancode &&
                            (nextEvent.xkey.time - storage->event.xkey.time) < 20)
                            return;
                    }

                    Input::KeyInfo keyInfo(
                        scancode,
                        m_App->storage->keycodes[scancode],
                        false
                    );

                    SpawnInput(Input::InputType::KEY, 
                    [&](Input::InputListener* pInputListener)
                    {
                        pInputListener->OnKey(keyInfo);
                    });

                    storage->lastPressed = UINT32_MAX;
                }
                break;
                
                case MotionNotify:
                {
                    if (!prevReceivedMouseMove)
                    {
                        mouseX = storage->event.xmotion.x_root;
                        mouseY = storage->event.xmotion.y_root;
                        relMouseX = storage->event.xmotion.x;
                        relMouseY = storage->event.xmotion.y;
                    }

                    MouseMoveInfo mouseMove(
                        storage->event.xmotion.x_root,
                        storage->event.xmotion.y_root,
                        storage->event.xmotion.x,
                        storage->event.xmotion.y,
                        relMouseX,
                        relMouseY,
                        mouseX,
                        mouseY
                    );
                    
                    SpawnInput(Input::InputType::MOUSE_MOVE, 
                    [&](Input::InputListener* pInputListener)
                    {
                        pInputListener->OnMouseMove(mouseMove);
                    });

                    mouseX = storage->event.xmotion.x_root;
                    mouseY = storage->event.xmotion.y_root;
                    relMouseX = storage->event.xmotion.x;
                    relMouseY = storage->event.xmotion.y;

                    prevReceivedMouseMove = true;
                }
                break;

                case ConfigureNotify:
                {
                    XConfigureEvent xce = storage->event.xconfigure;

                    // Verify that the event was a move event
                    if (xce.width != prevWidth || xce.height != prevHeight)
                    {
                        WindowResizeEvent linkEvent(
                            xce.width,
                            xce.height
                        );

                        SpawnEvent(Events::EventType::WINDOW_RESIZE, 
                        [&](Events::EventHandler* pEventHandler)
                        {
                            pEventHandler->OnWindowResize(linkEvent);
                        });

                        prevWidth = xce.width;
                        prevHeight = xce.height;

                        return;
                    }

                    if (xce.x != prevX || xce.y != prevY)
                    {
                        WindowMoveEvent event(
                            xce.x,
                            xce.y
                        );

                        SpawnEvent(Events::EventType::WINDOW_MOVE,
                        [&](Events::EventHandler* pEventHandler)
                        {
                            pEventHandler->OnWindowMove(event);
                        });

                        prevX = xce.x;
                        prevY = xce.y;
                    }
                }
                break;

                case ClientMessage:
                {
                    // Check for WM_PROTOCOLS
                    if (storage->event.xclient.message_type != 
                        XInternAtom(xdisplay, "WM_PROTOCOLS", false))
                        break;
                    
                    m_CloseRequested = true;

                    SpawnEvent(Events::EventType::WINDOW_CLOSING, 
                    [this](Events::EventHandler* pEventHandler)
                    {
                        pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
                    });
                }
                break;
            }
        }
    }

    void X11Window::ProcessMwmFunctions()
    {
        Atom motifWmHints = XInternAtom(xdisplay, "_MOTIF_WM_HINTS", true);

        MwmHints hints{};
        hints.flags = MWM_HINTS_FUNCTIONS;
        hints.functions = MWM_FUNC_MOVE;

        if (closable)
            hints.functions |= MWM_FUNC_CLOSE;
        if (resizable)
            hints.functions |= MWM_FUNC_RESIZE;
        if (minimizeBox)
            hints.functions |= MWM_FUNC_MINIMIZE;
        if (maximizeBox)
            hints.functions |= MWM_FUNC_MAXIMIZE;
        
        XChangeProperty(
            xdisplay, m_Window,
            motifWmHints,
            motifWmHints, 
            32,
            PropModeReplace,
            (unsigned char*)&hints,
            sizeof(hints) / sizeof(long)
        );
    }

}