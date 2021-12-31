#ifdef __linux__

#include <Tether/Application.hpp>
#include <Tether/Window.hpp>
#include <Tether/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

static bool initializedGlad = false;

bool Tether::Window::Init(
    int	x,
    int	y,
    unsigned int width,
    unsigned int height,
    bool showAfterInit,
    bool stripped
)
{
    if (initialized)
    {
        DispatchNoInit("Window::Init");
        return false;
    }
    
    if (!Application::IsInitialized())
        if (!Application::Init())
            return false;
    
    Display* display = Application::GetDisplay();
    int screen = Application::GetScreen();
    
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->stripped = stripped;

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

    if (!stripped)
        if (!InitGraphics())
            return false;
    
    if (showAfterInit)
    {
        XMapWindow(display, window);
        visible = true;
    }

    initialized = true;
    InitializeComponent();

    Repaint();

    return true;
}

void Tether::Window::SetVisible(bool visibility)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetVisible");
        return;
    }
    
    if (visibility)
        XMapWindow(Application::GetDisplay(), window);
    else
        XUnmapWindow(Application::GetDisplay(), window);
    
    visible = visibility;
}

bool Tether::Window::IsVisible()
{
    return visible && initialized;
}

void Tether::Window::SetX(int64_t x)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetX");
        return;
    }
    
    XMoveWindow(Application::GetDisplay(), window, x, GetY());
}

void Tether::Window::SetY(int64_t y)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetY");
        return;
    }

    XMoveWindow(Application::GetDisplay(), window, GetX(), y);
}

void Tether::Window::SetPosition(int64_t x, int64_t y)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetPosition");
        return;
    }

    XMoveWindow(Application::GetDisplay(), window, x, y);
}

void Tether::Window::SetWidth(uint64_t width)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetWidth");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, width, this->height);
}

void Tether::Window::SetHeight(uint64_t height)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetHeight");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, this->width, height);
}

void Tether::Window::SetSize(uint64_t width, uint64_t height)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetSize");
        return;
    }

    XResizeWindow(Application::GetDisplay(), window, width, height);
}

void Tether::Window::SetTitle(const char* title)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetTitle");
        return;
    }

    XStoreName(Application::GetDisplay(), window, title);
}

int64_t Tether::Window::GetX()
{
    if (!initialized)
    {
        DispatchNoInit("Window::GetX");
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

int64_t Tether::Window::GetY()
{
    if (!initialized)
    {
        DispatchNoInit("Window::GetY");
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

void Tether::Window::PollEvents()
{
    using namespace Events;

    if (!initialized)
    {
        DispatchNoInit("Window::PollEvents");
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

        Repaint();

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
                    mouseX,
                    mouseY,
                    relMouseX,
                    relMouseY
                );
                
                SpawnEvent(Events::EventType::MOUSE_MOVE, 
                [&](Events::EventHandler* pEventHandler)
                {
                    pEventHandler->OnMouseMove(linkEvent);
                });

                if (prevReceivedMouseMove)
                {
                    mouseX = event.xmotion.x_root;
                    mouseY = event.xmotion.y_root;
                    relMouseX = event.xmotion.x;
                    relMouseY = event.xmotion.y;
                }

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

    if (eventReceived)
        Repaint();
}

uint64_t Tether::Window::GetHandle()
{
    if (!initialized)
    {
        DispatchNoInit("Window::GetHandle");
        return 0;
    }

    return window;
}

void Tether::Window::OnDispose()
{
    XUnmapWindow(Application::GetDisplay(), window);

    DisposeGraphics();

    XDestroyWindow(Application::GetDisplay(), window);
}

void Tether::Window::SwapBuffers()
{
    if (!initialized)
    {
        DispatchNoInit("Window::SwapBuffers");
        return;
    }


}

#endif //__linux__