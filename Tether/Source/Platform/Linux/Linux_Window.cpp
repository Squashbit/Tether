#ifdef __linux__

#include <Tether/Window.hpp>
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

#define xdisplay app->storage->display
#define xscreen app->storage->screen

bool Tether::Window::Init(uint64_t width, uint64_t height, const char* title)
{
    m_App = &Application::Get();

    // Check if initialized
    // Not possible to use TETHER_ASSERT_INITIALIZED here
    if (initialized)
    {
        DispatchNoInit("Window::Init");
        return false;
    }

    if (!m_App->IsInitialized() && !m_App->Init())
        return false;

    this->setWidth = width;
    this->setHeight = height;

    bool shouldShow = true;
    for (uint64_t i = 0; i < hints.size(); i++)
    {
        int64_t value = hints[i].value;
        switch (hints[i].type)
        {
            case HintType::X: this->setX = value; break;
            case HintType::Y: this->setY = value; break;
            case HintType::VISIBLE: shouldShow = (bool)value; break;
        }
    }
    
    unsigned long root = RootWindow(xdisplay, xscreen);
    
    XSetWindowAttributes swa{};
    swa.event_mask = 
          PointerMotionMask 
        | ButtonPressMask
        | ButtonReleaseMask
        | StructureNotifyMask
        | KeyPressMask
        | KeyReleaseMask;

    storage->window = XCreateWindow(
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
    XSetWMProtocols(xdisplay, storage->window, &wmDelete, 1);

    if (shouldShow)
    {
        XMapWindow(xdisplay, storage->window);
        visible = true;
    }
    
    XMoveWindow(xdisplay, storage->window, setX, setY);
    XStoreName(xdisplay, storage->window, title);

    XFlush(xdisplay);

    initialized = true;
    OnInit();

    return true;
}

void Tether::Window::SetVisible(bool visibility)
{
    TETHER_ASSERT_INITIALIZED("Window::SetVisible");

    if (visibility)
        XMapWindow(xdisplay, storage->window);
    else
        XUnmapWindow(xdisplay, storage->window);
    
    visible = visibility;
}

bool Tether::Window::IsVisible()
{
    return visible && initialized;
}

void Tether::Window::SetX(int64_t x)
{
    TETHER_ASSERT_INITIALIZED("Window::SetX");

    XMoveWindow(xdisplay, storage->window, x, GetY());
}

void Tether::Window::SetY(int64_t y)
{
    TETHER_ASSERT_INITIALIZED("Window::SetY");

    XMoveWindow(xdisplay, storage->window, GetX(), y);
}

void Tether::Window::SetPosition(int64_t x, int64_t y)
{
    TETHER_ASSERT_INITIALIZED("Window::SetPosition");

    XMoveWindow(xdisplay, storage->window, x, y);
}

void Tether::Window::SetWidth(uint64_t width)
{
    TETHER_ASSERT_INITIALIZED("Window::SetWidth");

    if (width == 0)
        return;

    XResizeWindow(xdisplay, storage->window, width, GetHeight());
}

void Tether::Window::SetHeight(uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("Window::SetHeight");

    if (height == 0)
        return;

    XResizeWindow(xdisplay, storage->window, GetWidth(), height);
}

void Tether::Window::SetSize(uint64_t width, uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("Window::SetSize");

    if (width == 0 || height == 0)
        return;

    XResizeWindow(xdisplay, storage->window, width, height);
}

void Tether::Window::SetTitle(const char* title)
{
    TETHER_ASSERT_INITIALIZED("Window::SetTitle");

    XStoreName(xdisplay, storage->window, title);
}

void Tether::Window::SetBoundsEnabled(bool enabled)
{
    if (this->boundsEnabled == enabled)
        return;
    this->boundsEnabled = enabled;

    if (enabled)
        SetBounds(minWidth, minHeight, maxWidth, maxHeight);
    else
        SetBounds(INT32_MIN, INT32_MIN, INT32_MAX, INT32_MAX);
}

void Tether::Window::SetBounds(int64_t minWidth, int64_t minHeight, 
    int64_t maxWidth, int64_t maxHeight)
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

    XSetWMSizeHints(xdisplay, storage->window, &sizeHints, 
        XA_WM_NORMAL_HINTS);
}

void Tether::Window::SetPreferredResizeInc(int width, int height)
{
    XSizeHints sizeHints{};
    sizeHints.width_inc = width;
    sizeHints.height_inc = height;
    sizeHints.flags = PResizeInc;

    XSetWMSizeHints(xdisplay, storage->window, &sizeHints, 
        XA_WM_NORMAL_HINTS);
}

void Tether::Window::SetDecorated(bool decorated)
{
    Atom motifWmHints = XInternAtom(xdisplay, "_MOTIF_WM_HINTS", true);

    MwmHints hints{};
    hints.flags = MWM_HINTS_DECORATIONS;
    hints.decorations = decorated;

    XChangeProperty(
        xdisplay, storage->window,
        motifWmHints,
        motifWmHints, 
        32,
        PropModeReplace,
        (unsigned char*)&hints,
        sizeof(hints) / sizeof(long)
    );
}

void Tether::Window::SetClosable(bool closable)
{
    if (this->closable == closable)
        return;
    this->closable = closable;

    ProcessMwmFunctions();
}

void Tether::Window::SetResizable(bool isResizable)
{
    if (isResizable == resizable)
        return;
    resizable = isResizable;

    ProcessMwmFunctions();
}

void Tether::Window::SetMinimizeBox(bool enabled)
{
    if (this->minimizeBox == enabled)
        return;
    this->minimizeBox = enabled;

    ProcessMwmFunctions();
}

void Tether::Window::SetMaximized(bool maximized)
{
    TETHER_ASSERT_INITIALIZED("Window::SetMaximized");

    Atom wmState = XInternAtom(xdisplay, "_NET_WM_STATE", true);
    Atom maxHorzAtom = XInternAtom(xdisplay, 
        "_NET_WM_STATE_MAXIMIZED_HORZ", true);
    Atom maxVertAtom = XInternAtom(xdisplay, 
        "_NET_WM_STATE_MAXIMIZED_VERT", true);
    if (maximized)
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = storage->window;
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
        fullscreenEvent.xclient.window = storage->window;
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

void Tether::Window::SetMaximizeBox(bool enabled)
{
    if (this->maximizeBox == enabled)
        return;
    this->maximizeBox = enabled;

    ProcessMwmFunctions();
}

void Tether::Window::SetFullscreen(
    bool fullscreen, 
	FullscreenSettings* settings,
	Devices::Monitor* monitor
)
{
    TETHER_ASSERT_INITIALIZED("Window::SetFullscreen");

    if (this->fullscreen == fullscreen)
        return;

    Atom wmState = XInternAtom(xdisplay, "_NET_WM_STATE", true);
    Atom fullscreenAtom = XInternAtom(xdisplay, 
        "_NET_WM_STATE_FULLSCREEN", true);
    if (fullscreen)
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = storage->window;
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
        event.xclient.window = storage->window;
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
        fullscreenEvent.xclient.window = storage->window;
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

void Tether::Window::SetRawInputEnabled(bool enabled)
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

void Tether::Window::SetCursorMode(Window::CursorMode mode)
{
    switch (mode)
    {
        case Window::CursorMode::NORMAL:
        {
            XUngrabPointer(xdisplay, CurrentTime);
            XUndefineCursor(xdisplay, storage->window);
        }
        break;

        case Window::CursorMode::HIDDEN:
        {
            XDefineCursor(xdisplay, storage->window, 
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

void Tether::Window::SetCursorPos(int x, int y)
{
    XWarpPointer(xdisplay, storage->window, storage->window, 0, 0, 
        0, 0, x, y);
}

void Tether::Window::SetCursorRootPos(int x, int y)
{
    unsigned int root = DefaultRootWindow(xdisplay);
    XWarpPointer(xdisplay, root, root, 0, 0, 0, 0, x, y);
}

int64_t Tether::Window::GetX()
{
    TETHER_ASSERT_INITIALIZED_RET("Window::GetX", 0);

    long unsigned int child;

    int x, y;
    XTranslateCoordinates(xdisplay, storage->window, 
        DefaultRootWindow(xdisplay), 0, 0, &x, &y, &child);
    
    return x;
}

int64_t Tether::Window::GetY()
{
    TETHER_ASSERT_INITIALIZED_RET("Window::GetY", 0);

    long unsigned int child;

    int x, y;
    XTranslateCoordinates(xdisplay, storage->window, 
        DefaultRootWindow(xdisplay), 0, 0, &x, &y, &child);
    
    return y;
}

uint64_t Tether::Window::GetWidth()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(xdisplay, storage->window, &attribs);

    return attribs.width;
}

uint64_t Tether::Window::GetHeight()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(xdisplay, storage->window, &attribs);

    return attribs.height;
}

void Tether::Window::PollEvents()
{
    using namespace Events;
    using namespace Input;

    if (!initialized)
    {
        DispatchNoInit("Window::PollEvents");
        return;
    }
    
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

void Tether::Window::OnDispose()
{
    XLockDisplay(xdisplay);
        XUnmapWindow(xdisplay, storage->window);
        XDestroyWindow(xdisplay, storage->window);
        XFlush(xdisplay);
    XUnlockDisplay(xdisplay);

    hints.clear();
}

void Tether::Window::ProcessMwmFunctions()
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
        xdisplay, storage->window,
        motifWmHints,
        motifWmHints, 
        32,
        PropModeReplace,
        (unsigned char*)&hints,
        sizeof(hints) / sizeof(long)
    );
}

#endif //__linux__