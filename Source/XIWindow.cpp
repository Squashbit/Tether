#ifdef __linux__

#include <Tether/IWindow.hpp>
#include <Tether/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

static bool initializedGlad = false;

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
    for (uint64_t i = 0; i < hints.size(); i++)
    {
        int64_t value = hints[i].value;
        switch (hints[i].type)
        {
            case HintType::X: this->x = value; break;
            case HintType::Y: this->y = value; break;
            case HintType::VISIBLE: shouldShow = (bool)value; break;
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

    SetState(WindowState::NORMAL);
    SetState(WindowState::NORMAL);

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

void Tether::IWindow::SetState(WindowState state)
{
    TETHER_ASSERT_INITIALIZED("IWindow::SetState");

    Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
    switch (state)
    {
        case WindowState::NORMAL:
        {
            XDeleteProperty(display, window, wmState);
        }
        break;

        case WindowState::FULLSCREEN:
        {
            Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", 
                true);
            XChangeProperty(display, window, wmState, XA_ATOM, 32, 
                PropModeReplace, (unsigned char*)&fullscreen, 1);
        }
        break;
    }
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

#endif //__linux__