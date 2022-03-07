#ifdef __linux__

#ifdef TETHER_XRAWINPUT
#include <X11/extensions/XInput2.h>
#endif // TETHER_XRAWINPUT

#ifdef TETHER_MONITORS
#include <X11/extensions/Xrandr.h>
#endif // TETHER_MONITORS

#include <Tether/IWindow.hpp>
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

bool Tether::IWindow::Init(uint64_t width, uint64_t height, const char* title)
{
    // Check if initialized
    // Not possible to use TETHER_ASSERT_INITIALIZED here
    if (initialized)
    {
        DispatchNoInit("IWindow::Init");
        return false;
    }
    
    display = XOpenDisplay(NULL);
    if (!display)
        return false;
    
    int screen = DefaultScreen(display);

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
    
    unsigned long root = RootWindow(display, screen);

    XSetWindowAttributes swa{};
    swa.event_mask = 
          PointerMotionMask 
        | ButtonPressMask
        | ButtonReleaseMask
        | StructureNotifyMask;

    window = XCreateWindow(
        display, 
        root,
        setX, setY,
        width, height,
        0, // Border width
        DefaultDepth(display, screen),
        InputOutput,
        DefaultVisual(display, screen),
        CWEventMask,
        &swa
    );

    XColor color = {};
    hiddenCursorPixmap = XCreatePixmap(display, root, 1, 1, 1);
    hiddenCursor = XCreatePixmapCursor(display, 
        hiddenCursorPixmap, hiddenCursorPixmap, &color, &color, 0, 0);
    
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, window, &wmDelete, 1);

    if (shouldShow)
    {
        XMapWindow(display, window);
        visible = true;
    }
    
    XMoveWindow(display, window, setX, setY);
    XStoreName(display, window, title);

    XFlush(display);

    initialized = true;
    OnInit();

    return true;
}

#ifdef TETHER_MONITORS

uint64_t Tether::IWindow::GetMonitorCount()
{
    int numMonitors;
    XRRGetMonitors(display, window, false, &numMonitors);

    return numMonitors;
}

bool Tether::IWindow::GetMonitor(uint64_t index, Monitor* pMonitor)
{
    TETHER_ASSERT_INITIALIZED_RET("IWindow::GetMonitor", false);

    if (!pMonitor)
        return false;

    unsigned long root = RootWindow(display, screen);
    XRRScreenResources* resources = XRRGetScreenResources(display, root);

    int numMonitors;
    XRRMonitorInfo* monitorInfos = XRRGetMonitors(display, window, false, 
        &numMonitors);

    if (index >= numMonitors)
        return false;
    
    XRRMonitorInfo monitorInfo = monitorInfos[index];

    pMonitor->modes.clear();
    for (uint64_t i = 0; i < monitorInfo.noutput; i++)
    {
        XRROutputInfo* outputInfo = XRRGetOutputInfo(display, resources,
            monitorInfo.outputs[i]);
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, 
            outputInfo->crtc);
        
        for (uint64_t i2 = 0; i2 < outputInfo->nmode; i2++)
        {
            XRRModeInfo mode;
            for (uint64_t i3 = 0; i3 < resources->nmode; i3++)
                if (resources->modes[i3].id == outputInfo->modes[i2])
                    mode = resources->modes[i3];
            
            DisplayMode displayMode;
            displayMode.name = mode.name;
            displayMode.exactRefreshRate = mode.dotClock 
                    / ((double)mode.hTotal * mode.vTotal);
            displayMode.refreshRate = round(displayMode.exactRefreshRate);
            displayMode.width = mode.width;
            displayMode.height = mode.height;

            if (info->mode == mode.id)
                pMonitor->currentMode = displayMode;
            
            pMonitor->modes.push_back(displayMode);
        }

        XRRFreeCrtcInfo(info);
        XRRFreeOutputInfo(outputInfo);
    }

    char* name = XGetAtomName(display, monitorInfo.name);
    
    pMonitor->index = index;
    pMonitor->width = monitorInfo.width;
    pMonitor->height = monitorInfo.height;
    pMonitor->name = name;
    pMonitor->primary = monitorInfo.primary;

    XFree(name);
    XRRFreeMonitors(monitorInfos);
    XRRFreeScreenResources(resources);
    
    return true;
}

#endif // TETHER_MONITORS

void Tether::IWindow::SetVisible(bool visibility)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetVisible");

    if (visibility)
        XMapWindow(display, window);
    else
        XUnmapWindow(display, window);
    
    visible = visibility;
}

bool Tether::IWindow::IsVisible()
{
    return visible && initialized;
}

#ifdef TETHER_XRAWINPUT

void Tether::IWindow::SetRawInputEnabled(bool enabled)
{
    unsigned int root = DefaultRootWindow(display);

    if (enabled)
    {
        XIEventMask eventMask;
        unsigned char mask[XIMaskLen(XI_RawMotion)] = { 0 };

        eventMask.deviceid = XIAllMasterDevices;
        eventMask.mask_len = sizeof(mask);
        eventMask.mask = mask;
        XISetMask(mask, XI_RawMotion);

        XISelectEvents(display, root, &eventMask, 1);

        return;
    }

    XIEventMask eventMask;
    unsigned char mask[] = { 0 };

    eventMask.deviceid = XIAllMasterDevices;
    eventMask.mask_len = sizeof(mask);
    eventMask.mask = mask;

    XISelectEvents(display, root, &eventMask, 1);
}

#endif // TETHER_XRAWINPUT

void Tether::IWindow::SetCursorMode(CursorMode mode)
{
    switch (mode)
    {
        case CursorMode::NORMAL:
        {
            XUngrabPointer(display, CurrentTime);
        }
        break;

        case CursorMode::HIDDEN:
        {
            XGrabPointer(
                display, DefaultRootWindow(display), 0,
                PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
                GrabModeAsync, GrabModeAsync, None, hiddenCursor, CurrentTime
            );
        }
        break;
    }
}

void Tether::IWindow::SetMousePos(uint64_t x, uint64_t y)
{
    XWarpPointer(display, window, window, 0, 0, 0, 0, x, y);
}

void Tether::IWindow::SetMouseRootPos(uint64_t x, uint64_t y)
{
    unsigned int root = DefaultRootWindow(display);
    XWarpPointer(display, root, root, 0, 0, 0, 0, x, y);
}

void Tether::IWindow::SetX(int64_t x)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetX");

    XMoveWindow(display, window, x, GetY());
}

void Tether::IWindow::SetY(int64_t y)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetY");

    XMoveWindow(display, window, GetX(), y);
}

void Tether::IWindow::SetPosition(int64_t x, int64_t y)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetPosition");

    XMoveWindow(display, window, x, y);
}

void Tether::IWindow::SetWidth(uint64_t width)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetWidth");

    if (width == 0)
        return;

    XResizeWindow(display, window, width, GetHeight());
}

void Tether::IWindow::SetHeight(uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetHeight");

    if (height == 0)
        return;

    XResizeWindow(display, window, GetWidth(), height);
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetSize");

    if (width == 0 || height == 0)
        return;

    XResizeWindow(display, window, width, height);
}

void Tether::IWindow::SetTitle(const char* title)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetTitle");

    XStoreName(display, window, title);
}

void Tether::IWindow::SetBoundsEnabled(bool enabled)
{
    if (this->boundsEnabled == enabled)
        return;
    this->boundsEnabled = enabled;

    if (enabled)
        SetBounds(minWidth, minHeight, maxWidth, maxHeight);
    else
        SetBounds(INT32_MIN, INT32_MIN, INT32_MAX, INT32_MAX);
}

void Tether::IWindow::SetBounds(int64_t minWidth, int64_t minHeight, 
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

    XSetWMSizeHints(display, window, &sizeHints, XA_WM_NORMAL_HINTS);
}

void Tether::IWindow::SetPreferredResizeInc(int width, int height)
{
    XSizeHints sizeHints{};
    sizeHints.width_inc = width;
    sizeHints.height_inc = height;
    sizeHints.flags = PResizeInc;

    XSetWMSizeHints(display, window, &sizeHints, XA_WM_NORMAL_HINTS);
}

void Tether::IWindow::SetDecorated(bool decorated)
{
    Atom motifWmHints = XInternAtom(display, "_MOTIF_WM_HINTS", true);

    MwmHints hints{};
    hints.flags = MWM_HINTS_DECORATIONS;
    hints.decorations = decorated;

    XChangeProperty(
        display, window,
        motifWmHints,
        motifWmHints, 
        32,
        PropModeReplace,
        (unsigned char*)&hints,
        sizeof(hints) / sizeof(long)
    );
}

void Tether::IWindow::SetClosable(bool closable)
{
    if (this->closable == closable)
        return;
    this->closable = closable;

    ProcessMwmFunctions();
}

void Tether::IWindow::SetResizable(bool isResizable)
{
    if (isResizable == resizable)
        return;
    resizable = isResizable;

    ProcessMwmFunctions();
}

void Tether::IWindow::SetMinimizeBox(bool enabled)
{
    if (this->minimizeBox == enabled)
        return;
    this->minimizeBox = enabled;

    ProcessMwmFunctions();
}

void Tether::IWindow::SetMaximized(bool maximized)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetMaximized");

    Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
    Atom maxHorzAtom = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", 
            true);
    Atom maxVertAtom = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", 
            true);
    if (maximized)
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = window;
        fullscreenEvent.xclient.message_type = wmState;
        fullscreenEvent.xclient.format = 32;
        fullscreenEvent.xclient.data.l[0] = 1; // Add
        fullscreenEvent.xclient.data.l[1] = maxHorzAtom;
        fullscreenEvent.xclient.data.l[2] = maxVertAtom;
        
        XSendEvent(display, DefaultRootWindow(display), false, 
            SubstructureRedirectMask | SubstructureNotifyMask, 
            &fullscreenEvent);
    }
    else
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = window;
        fullscreenEvent.xclient.message_type = wmState;
        fullscreenEvent.xclient.format = 32;
        fullscreenEvent.xclient.data.l[0] = 0; // Remove
        fullscreenEvent.xclient.data.l[1] = maxHorzAtom;
        fullscreenEvent.xclient.data.l[2] = maxVertAtom;
        
        XSendEvent(display, DefaultRootWindow(display), false, 
            SubstructureRedirectMask | SubstructureNotifyMask, 
            &fullscreenEvent);
    }

    XSync(display, false);
}

void Tether::IWindow::SetMaximizeBox(bool enabled)
{
    if (this->maximizeBox == enabled)
        return;
    this->maximizeBox = enabled;

    ProcessMwmFunctions();
}

void Tether::IWindow::SetFullscreen(
    bool fullscreen, 
	FullscreenSettings* settings,
	Monitor* monitor
)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetFullscreen");

    if (this->fullscreen == fullscreen)
        return;

    Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
    Atom fullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", 
            true);
    if (fullscreen)
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = window;
        fullscreenEvent.xclient.message_type = wmState;
        fullscreenEvent.xclient.format = 32;
        fullscreenEvent.xclient.data.l[0] = 2; // Replace
        fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
        
        XSendEvent(display, DefaultRootWindow(display), false, 
            SubstructureRedirectMask | SubstructureNotifyMask, 
            &fullscreenEvent);
        
        XSync(display, false);
        
        Atom wmFullscreenMonitors = 
            XInternAtom(display, "_NET_WM_FULLSCREEN_MONITORS", true);
        XEvent event{};
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = wmFullscreenMonitors;
        event.xclient.format = 32;

        if (monitor)
        {
            event.xclient.data.l[0] = monitor->index;
            event.xclient.data.l[1] = monitor->index;
            event.xclient.data.l[2] = monitor->index;
            event.xclient.data.l[3] = monitor->index;
        }

        XSendEvent(display, DefaultRootWindow(display), false, 
            SubstructureRedirectMask | SubstructureNotifyMask, &event);
    }
    else
    {
        XEvent fullscreenEvent{};
        fullscreenEvent.type = ClientMessage;
        fullscreenEvent.xclient.window = window;
        fullscreenEvent.xclient.message_type = wmState;
        fullscreenEvent.xclient.format = 32;
        fullscreenEvent.xclient.data.l[0] = 0; // Remove
        fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
        
        XSendEvent(display, DefaultRootWindow(display), false, 
            SubstructureRedirectMask | SubstructureNotifyMask, 
            &fullscreenEvent);
    }

    XSync(display, false);
    
    this->fullscreen = fullscreen;
}

int64_t Tether::IWindow::GetX()
{
    TETHER_ASSERT_INITIALIZED_RET("IWindow::GetX", 0);

    long unsigned int child;

    int x, y;
    XTranslateCoordinates(display, window, 
        DefaultRootWindow(display), 0, 0, &x, &y, &child);
    
    return x;
}

int64_t Tether::IWindow::GetY()
{
    TETHER_ASSERT_INITIALIZED_RET("IWindow::GetY", 0);

    long unsigned int child;

    int x, y;
    XTranslateCoordinates(display, window, 
        DefaultRootWindow(display), 0, 0, &x, &y, &child);
    
    return y;
}

uint64_t Tether::IWindow::GetWidth()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);

    return attribs.width;
}

uint64_t Tether::IWindow::GetHeight()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);

    return attribs.height;
}

void Tether::IWindow::PollEvents()
{
    using namespace Events;

    if (!initialized)
    {
        DispatchNoInit("IWindow::PollEvents");
        return;
    }
    
    if (!visible)
        return;

    bool eventReceived = false;
    while (XPending(display))
    {
        XNextEvent(display, &event);
        eventReceived = true;
        
        switch (event.type)
        {
            case MotionNotify:
            {
                if (!prevReceivedMouseMove)
                {
                    mouseX = event.xmotion.x_root;
                    mouseY = event.xmotion.y_root;
                    relMouseX = event.xmotion.x;
                    relMouseY = event.xmotion.y;
                }

                MouseMoveEvent linkEvent(
                    event.xmotion.x_root,
                    event.xmotion.y_root,
                    event.xmotion.x,
                    event.xmotion.y,
                    relMouseX,
                    relMouseY,
                    mouseX,
                    mouseY
                );
                
                SpawnEvent(Events::EventType::MOUSE_MOVE, 
                [&](Events::EventHandler* pEventHandler)
                {
                    pEventHandler->OnMouseMove(linkEvent);
                });

                mouseX = event.xmotion.x_root;
                mouseY = event.xmotion.y_root;
                relMouseX = event.xmotion.x;
                relMouseY = event.xmotion.y;

                prevReceivedMouseMove = true;
            }
            break;

            case ConfigureNotify:
            {
                XConfigureEvent xce = event.xconfigure;

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
                if (event.xclient.message_type != 
                    XInternAtom(display, "WM_PROTOCOLS", false))
                    break;
                
                closeRequested = true;

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

Display* Tether::IWindow::GetDisplay()
{
    return display;
}

int Tether::IWindow::GetScreen()
{
    return screen;
}

uint64_t Tether::IWindow::GetHandle()
{
    TETHER_ASSERT_INITIALIZED_RET("IWindow::GetHandle", 0);

    return window;
}

void Tether::IWindow::OnDispose()
{
    XFreeCursor(display, hiddenCursor);
    XFreePixmap(display, hiddenCursorPixmap);

    XLockDisplay(display);
        XUnmapWindow(display, window);
        XDestroyWindow(display, window);
        XFlush(display);
    XUnlockDisplay(display);

    XCloseDisplay(display);

    hints.clear();
}

void Tether::IWindow::ProcessMwmFunctions()
{
    Atom motifWmHints = XInternAtom(display, "_MOTIF_WM_HINTS", true);

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
        display, window,
        motifWmHints,
        motifWmHints, 
        32,
        PropModeReplace,
        (unsigned char*)&hints,
        sizeof(hints) / sizeof(long)
    );
}

#endif //__linux__