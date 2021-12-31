#include <Tether/Application.hpp>

using namespace Tether;

bool Application::Init()
{
    return app.Init();
}

bool Application::IsInitialized()
{
    return app.IsInitialized();
}

Display* Application::GetDisplay()
{
    return app.GetDisplay();
}

Atom* Application::GetWMDeleteAtom()
{
    return app.GetWMDeleteAtom();
}

int Application::GetScreen()
{
    return app.GetScreen();
}

void Application::Dispose()
{
    app.Dispose();
}

ApplicationInternal::~ApplicationInternal()
{
    Dispose();
}

bool ApplicationInternal::Init()
{
    if (initialized)
        return false;
    
    display = XOpenDisplay(NULL);
    if (!display)
        return false;
    
    screen = DefaultScreen(display);
    wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    
    initialized = true;
    return true;
}

bool ApplicationInternal::IsInitialized()
{
    return initialized;
}

Display* ApplicationInternal::GetDisplay()
{
    return display;
}

Atom* ApplicationInternal::GetWMDeleteAtom()
{
    return &wmDelete;
}

int ApplicationInternal::GetScreen()
{
    return screen;
}

void ApplicationInternal::Dispose()
{
    if (!initialized)
        return;
    
    XCloseDisplay((Display*)display);
    
    initialized = false;
}