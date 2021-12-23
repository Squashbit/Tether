#ifdef __linux__

#include <Link/Window.hpp>
#include <Link/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

static bool initializedGlad = false;

bool Link::Window::Init(
    int	x,
    int	y,
    unsigned int width,
    unsigned int height,
    bool stripped
)
{
    if (initialized)
        return false;
    
    display = XOpenDisplay(NULL);
    if (!display)
        return false;
    
    wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->stripped = stripped;
    
    screen = DefaultScreen(display);
    
    GLint attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    if (!stripped)
    {
        visual = glXChooseVisual(display, screen, attribs);
        if (!visual)
            return false;
    }

    unsigned long root = RootWindow(display, this->screen);

    cmap = XCreateColormap(display, root, visual->visual, AllocNone);

    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = 0xFFFF;

    window = XCreateWindow(
        display, 
        root,
        x, y,
        width, height,
        0, // Border width
        visual->depth,
        InputOutput,
        visual->visual,
        CWColormap | CWEventMask,
        &swa
    );

    XSetWMProtocols(display, window, &wmDelete, 1);
    XFlush(display);

    if (!stripped)
    {
        // Initialize glX context
        glxContext = glXCreateContext(display, visual, NULL, GL_TRUE);
        // Make context current
        glXMakeCurrent(display, window, glxContext);

        if (!initializedGlad)
        {
            if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
                return false;
            
            initializedGlad = true;
        }

        InitGraphics();
    }

    XMapWindow(display, window);

    initialized = true;
    InitializeComponent();

    Repaint();

    return true;
}

void Link::Window::MakeCurrent()
{
    if (!initialized)
        return;
    
    glXMakeCurrent(display, window, glxContext);
}

void Link::Window::Show()
{
    if (!initialized)
        return;

    XMapWindow(display, window);
}

void Link::Window::Hide()
{
    if (!initialized)
        return;
    
    XUnmapWindow(display, window);
}

void Link::Window::SetX(int64_t x)
{
    if (!initialized)
        return;
    
    XMoveWindow(display, window, x, GetY());
}

void Link::Window::SetY(int64_t y)
{
    if (!initialized)
        return;

    XMoveWindow(display, window, GetX(), y);
}

void Link::Window::SetPosition(int64_t x, int64_t y)
{
    if (!initialized)
        return;

    XMoveWindow(display, window, x, y);
}

void Link::Window::SetWidth(uint64_t width)
{
    if (!initialized)
        return;

    XResizeWindow(display, window, width, this->height);
}

void Link::Window::SetHeight(uint64_t height)
{
    if (!initialized)
        return;

    XResizeWindow(display, window, this->width, height);
}

void Link::Window::SetSize(uint64_t width, uint64_t height)
{
    if (!initialized)
        return;

    XResizeWindow(display, window, width, height);
}

void Link::Window::SetTitle(const char* title)
{
    if (!initialized)
        return;

    XStoreName(display, window, title);
}

int64_t Link::Window::GetX()
{
    if (!initialized)
        return 0;

    XWindowAttributes attributes;
    XGetWindowAttributes(display, window, &attributes);
    
    return attributes.x;
}

int64_t Link::Window::GetY()
{
    if (!initialized)
        return 0;

    XWindowAttributes attributes;
    XGetWindowAttributes(display, window, &attributes);
    
    return attributes.y;
}

void Link::Window::PollEvents()
{
    using namespace Events;

    if (!initialized)
        return;

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

                if (handlers.count(EventType::MOUSE_MOVE))
                {
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

                    std::vector<Events::EventHandler*> eventList = 
                        handlers[EventType::MOUSE_MOVE];
                    
                    for (uint64_t i = 0; i < eventList.size(); i++)
                        eventList[i]->OnMouseMove(linkEvent);
                }

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
                if (handlers.count(EventType::WINDOW_RESIZE))
                {
                    Events::WindowResizeEvent linkEvent(
                        event.xexpose.width, 
                        event.xexpose.height
                    );

                    std::vector<Events::EventHandler*> eventList = 
                        handlers[EventType::WINDOW_RESIZE];

                    for (uint64_t i = 0; i < eventList.size(); i++)
                        eventList[i]->OnWindowResize(linkEvent);
                }

                width = event.xexpose.width;
                height = event.xexpose.height;
            }
            break;

            case ClientMessage:
            {
                closeRequested = true;

                if (handlers.count(EventType::WINDOW_CLOSING))
                {
                    std::vector<Events::EventHandler*> eventList = 
                        handlers[EventType::WINDOW_CLOSING];
                    
                    for (uint64_t i = 0; i < eventList.size(); i++)
                        eventList[i]->OnWindowClosing(
                            Events::WindowClosingEvent());
                }
            }
            break;
        }
    }

    if (eventReceived)
        Repaint();
}

void Link::Window::SetDrawingColor(Color color)
{
    if (!initialized || stripped)
        return;
}

void Link::Window::DrawRect(uint64_t x, uint64_t y, uint64_t width, 
    uint64_t height)
{
    if (!initialized || stripped)
        return;
}

uint64_t Link::Window::GetHandle()
{
    if (!initialized)
        return 0;

    return window;
}

Display* Link::Window::GetXDisplay()
{
    if (!initialized)
        return nullptr;

    return display;
}

void Link::Window::OnDispose()
{
    Hide();

    rectVertexBuffer.Dispose();
    rectVAO.Dispose();

    MakeCurrent();
    glXDestroyContext(display, glxContext);

    XDestroyWindow(display, window);
    XCloseDisplay((Display*)display);
}

void Link::Window::SwapBuffers()
{
    glXSwapBuffers(display, window);
}

#endif //__linux__