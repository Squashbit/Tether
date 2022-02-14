#ifdef __linux__

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

    this->width = width;
    this->height = height;

    bool shouldShow = true;
    bool fullscreenMode = false;
    for (uint64_t i = 0; i < hints.size(); i++)
    {
        int64_t value = hints[i].value;
        switch (hints[i].type)
        {
            case HintType::X: this->x = value; break;
            case HintType::Y: this->y = value; break;
            case HintType::VISIBLE: shouldShow = (bool)value; break;
            case HintType::FULLSCREEN: fullscreenMode = (bool)value; break;
        }
    }
    
    unsigned long root = RootWindow(display, screen);

    XSetWindowAttributes swa{};
    swa.event_mask = 
          PointerMotionMask 
        | StructureNotifyMask;

    window = XCreateWindow(
        display, 
        root,
        x, y,
        width, height,
        0, // Border width
        DefaultDepth(display, screen),
        InputOutput,
        DefaultVisual(display, screen),
        CWEventMask,
        &swa
    );
    
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, window, &wmDelete, 1);

    if (fullscreenMode)
    {
        fullscreen = true;

        Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
        Atom fullscreen = XInternAtom(display, 
            "_NET_WM_STATE_FULLSCREEN", true);
        XChangeProperty(display, window, wmState, XA_ATOM, 32, 
            PropModeReplace, (unsigned char*)&fullscreen, 1);
    }

    if (shouldShow)
    {
        XMapWindow(display, window);
        visible = true;
    }
    
    XMoveWindow(display, window, x, y);
    XStoreName(display, window, title);

    XSync(display, false);
    
    initialized = true;
    OnInit();

    return true;
}

void Tether::IWindow::SetVisible(bool visibility)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetVisible");

    if (visibility)
    {
        XMapWindow(display, window);
        XMoveWindow(display, window, x, y);
        XResizeWindow(display, window, this->width, height);
    }
    else
        XUnmapWindow(display, window);
    
    visible = visibility;
}

bool Tether::IWindow::IsVisible()
{
    return visible && initialized;
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

    XResizeWindow(display, window, width, this->height);
}

void Tether::IWindow::SetHeight(uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetHeight");

    XResizeWindow(display, window, this->width, height);
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetSize");

    XResizeWindow(display, window, width, height);
}

void Tether::IWindow::SetTitle(const char* title)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetTitle");

    XStoreName(display, window, title);
}

void Tether::IWindow::SetBounds(int minWidth, int minHeight, int maxWidth, 
    int maxHeight)
{
    XSizeHints sizeHints{};
    sizeHints.min_width = minWidth;
    sizeHints.min_height = minHeight;
    sizeHints.max_width = maxWidth;
    sizeHints.max_height = maxHeight;
    sizeHints.flags = PMinSize | PMaxSize;

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

void Tether::IWindow::SetFullscreen(bool fullscreen, int monitor)
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
        event.xclient.data.l[0] = monitor;
        event.xclient.data.l[1] = monitor;
        event.xclient.data.l[2] = monitor;
        event.xclient.data.l[3] = monitor;

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
    
    XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);
    
    return x - attribs.x;
}

int64_t Tether::IWindow::GetY()
{
    TETHER_ASSERT_INITIALIZED_RET("IWindow::GetY", 0);

    long unsigned int child;

    int x, y;
    XTranslateCoordinates(display, window, 
        DefaultRootWindow(display), 0, 0, &x, &y, &child);
    
    XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);
    
    return y - attribs.y;
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

                // Verify that the event was a resize event
                if (xce.width == width && xce.height == height)
                    break;
                
                WindowResizeEvent linkEvent(
                    xce.width,
                    xce.height
                );

                SpawnEvent(Events::EventType::WINDOW_RESIZE, 
                [&](Events::EventHandler* pEventHandler)
                {
                    pEventHandler->OnWindowResize(linkEvent);
                });

                width = xce.width;
                height = xce.height;
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
    hints.functions = MWM_FUNC_MOVE | MWM_FUNC_MAXIMIZE;

    if (closable)
        hints.functions |= MWM_FUNC_CLOSE;
    if (resizable)
        hints.functions |= MWM_FUNC_RESIZE;
    
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