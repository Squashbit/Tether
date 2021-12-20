#ifdef __linux__

#include <Link/Window.hpp>
#include <Link/Common/VectorUtils.hpp>
#include <Link/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

static bool initializedGlad = false;

Link::Window::Window()
{
    display = XOpenDisplay("");
    if (!display)
        return;
    
    wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
}

Link::Window::~Window()
{
    if (!initialized)
        return;
    
    Hide();

    MakeCurrent();
    glXDestroyContext(display, glxContext);

    XCloseDisplay((Display*)display);
}

Link::ErrorCode Link::Window::Init(
    int	x,
    int	y,
    unsigned int width,
    unsigned int height,
    bool stripped
)
{
    if (initialized)
        return ErrorCode::DOUBLE_INIT;
    if (!display)
        return ErrorCode::DISPLAY_NOT_FOUND;
    
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
            return ErrorCode::VISUAL_NOT_FOUND;
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

    XMapWindow(display, window);
    XSetWMProtocols(display, window, &wmDelete, 1);
    XFlush(display);

    if (!stripped)
    {
        // Initialize glX context
        glxContext = glXCreateContext(display, visual, NULL, GL_TRUE);
        // Make context current
        glXMakeCurrent(display, window, glxContext);
    }

    if (!initializedGlad || !stripped)
    {
        if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress))
            return ErrorCode::GL_INIT_FAILED;
        
        initializedGlad = true;
    }

    initialized = true;
    InitializeComponent();

    Repaint();

    return ErrorCode::SUCCESS;
}

void Link::Window::MakeCurrent()
{
    if (!initialized)
        return;
    
    glXMakeCurrent(display, window, glxContext);
}

void Link::Window::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void Link::Window::AddEventHandler(Events::EventHandler* handler,
    Events::EventType eventType)
{
    using namespace Events;

    switch (eventType)
    {
        case EventType::MOUSE_MOVE:
        {
            if (VectorUtils::Contains(mouseMoveHandlers, handler))
                break;
            
            mouseMoveHandlers.push_back(handler);
        }
        break;

        case EventType::WINDOW_CLOSING:
        {
            if (VectorUtils::Contains(windowClosingHandlers, handler))
                break;
            
            windowClosingHandlers.push_back(handler);
        }
        break;

        case EventType::WINDOW_RESIZE:
        {
            if (VectorUtils::Contains(windowResizeHandlers, handler))
                break;
            
            windowResizeHandlers.push_back(handler);
        }
        break;
    }
}

void Link::Window::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void Link::Window::RemoveEventHandler(Events::EventHandler* handler)
{
    VectorUtils::EraseAll(mouseMoveHandlers, handler);
    VectorUtils::EraseAll(windowClosingHandlers, handler);
    VectorUtils::EraseAll(windowResizeHandlers, handler);
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

uint64_t Link::Window::GetMouseX()
{
    return mouseX;
}

uint64_t Link::Window::GetMouseY()
{
    return mouseY;
}

uint64_t Link::Window::GetRelativeMouseX()
{
    return relMouseX;
}

uint64_t Link::Window::GetRelativeMouseY()
{
    return relMouseY;
}

uint64_t Link::Window::GetWidth()
{
    if (!initialized)
        return 0;

    return width;
}

uint64_t Link::Window::GetHeight()
{
    if (!initialized)
        return 0;

    return height;
}

void Link::Window::PollEvents()
{
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

                Events::MouseMoveEvent linkEvent(
                    event.xmotion.x_root,
                    event.xmotion.y_root,
                    event.xmotion.x,
                    event.xmotion.y,
                    mouseX,
                    mouseY,
                    relMouseX,
                    relMouseY
                );

                for (uint64_t i = 0; i < mouseMoveHandlers.size(); i++)
                    mouseMoveHandlers[i]->OnMouseMove(linkEvent);

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
                Events::WindowResizeEvent linkEvent(
                    event.xexpose.width, 
                    event.xexpose.height
                );

                for (uint64_t i = 0; i < windowResizeHandlers.size(); i++)
                    windowResizeHandlers[i]->OnWindowResize(linkEvent);

                width = event.xexpose.width;
                height = event.xexpose.height;
            }
            break;

            case ClientMessage:
            {
                closeRequested = true;
                for (uint64_t i = 0; i < windowClosingHandlers.size(); i++)
                    windowClosingHandlers[i]->OnWindowClosing(
                        Events::WindowClosingEvent());
            }
            break;
        }
    }

    if (eventReceived)
        Repaint();
}

void Link::Window::AddControl(Controls::Control* pControl)
{
    if (!initialized)
        return;

    AddControlNoRepaint(pControl);
    Repaint();
}

bool Link::Window::RemoveControl(Controls::Control* pControl)
{
    if (!initialized)
        return false;

    if (RemoveControlNoRepaint(pControl))
    {
        Repaint();
        return true;
    }

    return false;
}

void Link::Window::AddControlNoRepaint(Controls::Control* pControl)
{
    if (!initialized)
        return;

    controls.push_back(pControl);
}

bool Link::Window::RemoveControlNoRepaint(Controls::Control* pControl)
{
    if (!initialized)
        return false;

    for (uint64_t i = 0; i < controls.size(); i++)
        if (controls[i] == pControl)
        {
            controls.erase(controls.begin() + i);
            return true;
        }
    
    return false;
}

void Link::Window::SetBackgroundColor(Color backgroundColor)
{
    if (!initialized)
        return;

    this->backgroundColor = backgroundColor;
}

void Link::Window::ClearWindow()
{
    if (!initialized)
        return;

    XClearWindow(display, window);
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

void Link::Window::Repaint()
{
    if (!initialized || stripped)
        return;

    glViewport(GetX(), GetY(), width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -10000, 10000);
    
    glClearColor(
        backgroundColor.GetR(),
        backgroundColor.GetG(),
        backgroundColor.GetB(),
        1.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (uint64_t i = 0; i < controls.size(); i++)
        controls[i]->Render(this);
    
    glXSwapBuffers(display, window);
}

bool Link::Window::IsCloseRequested()
{
    if (!initialized)
        return false;

    return closeRequested;
}

void Link::Window::IgnoreClose()
{
    if (!initialized)
        return;

    closeRequested = false;
}

#endif //__linux__