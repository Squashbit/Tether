#ifdef __linux__

#include <Tether/Application.hpp>
#include <Tether/IWindow.hpp>
#include <Tether/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

static bool initializedGlad = false;

bool Tether::IWindow::Init(uint64_t width, uint64_t height, const char* title)
{
    if (initialized)
    {
        DispatchNoInit("IWindow::Init");
        return false;
    }
    
    if (!Application::IsInitialized())
        if (!Application::Init())
            return false;
    
    Display* display = Application::GetDisplay();
    int screen = Application::GetScreen();

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

    XSetWindowAttributes swa;
    swa.event_mask = 0xFFFF;

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

    XSetWMProtocols(display, window, Application::GetWMDeleteAtom(), 1);
    XFlush(display);

    if (shouldShow)
    {
        XMapWindow(display, window);
        visible = true;
    }

    XMoveWindow(display, window, x, y);
    XStoreName(display, window, title);

    initialized = true;
    OnInit();

    return true;
}

void Tether::IWindow::SetVisible(bool visibility)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetVisible");
        return;
    }

    Display* display = Application::GetDisplay();
    
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
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetX");
        return;
    }
    
    XMoveWindow(Application::GetDisplay(), window, x, GetY());
}

void Tether::IWindow::SetY(int64_t y)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetY");
        return;
    }

    XMoveWindow(Application::GetDisplay(), window, GetX(), y);
}

void Tether::IWindow::SetPosition(int64_t x, int64_t y)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetPosition");
        return;
    }

    XMoveWindow(Application::GetDisplay(), window, x, y);
}

void Tether::IWindow::SetWidth(uint64_t width)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetWidth");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, width, this->height);
}

void Tether::IWindow::SetHeight(uint64_t height)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetHeight");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, this->width, height);
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetSize");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, width, height);
}

void Tether::IWindow::SetTitle(const char* title)
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::SetTitle");
        return;
    }

    XStoreName(Application::GetDisplay(), window, title);
}

int64_t Tether::IWindow::GetX()
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::GetX");
        return 0;
    }

    Display* display = Application::GetDisplay();
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
    if (!initialized)
    {
        DispatchNoInit("IWindow::GetY");
        return 0;
    }

    Display* display = Application::GetDisplay();
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

    Display* display = Application::GetDisplay();

    bool eventReceived = false;
    while (XPending(display))
    {
        XNextEvent(display, &event);
        eventReceived = true;
        
        uint32_t queryRoot, queryChild;
        int absoluteX, absoluteY;
        int relativeX, relativeY;
        uint32_t modKeyMask;
        XDefs::QueryPointer(
            display, window,
            &queryRoot, &queryChild,
            &absoluteX, &absoluteY,
            &relativeX, &relativeY,
            &modKeyMask
        );

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

            case Expose:
            {
                SpawnEvent(Events::EventType::WINDOW_RESIZE, 
                [this](Events::EventHandler* pEventHandler)
                {
                    pEventHandler->OnWindowResize(Events::WindowResizeEvent());
                });

                width = event.xexpose.width;
                height = event.xexpose.height;
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

uint64_t Tether::IWindow::GetHandle()
{
    if (!initialized)
    {
        DispatchNoInit("IWindow::GetHandle");
        return 0;
    }

    return window;
}

void Tether::IWindow::OnDispose()
{
    XUnmapWindow(Application::GetDisplay(), window);

    XDestroyWindow(Application::GetDisplay(), window);

    hints.clear();
}

#endif //__linux__