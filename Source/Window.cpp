#include <Tether/Window.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;

void Tether::Window::AddControl(Controls::Control* pControl)
{
    if (!initialized)
    {
        DispatchNoInit("Window::AddControl");
        return;
    }

    AddControlNoRepaint(pControl);
    Repaint();
}

bool Tether::Window::RemoveControl(Controls::Control* pControl)
{
    if (!initialized)
    {
        DispatchNoInit("Window::RemoveControl");
        return false;
    }

    if (RemoveControlNoRepaint(pControl))
    {
        Repaint();
        return true;
    }

    return false;
}

void Tether::Window::AddControlNoRepaint(Controls::Control* pControl)
{
    if (!initialized)
    {
        DispatchNoInit("Window::AddControl or Window::AddControlNoRepaint");
        return;
    }

    controls.push_back(pControl);
}

bool Tether::Window::RemoveControlNoRepaint(Controls::Control* pControl)
{
    if (!initialized)
    {
        DispatchNoInit("Window::RemoveControl or Window::RemoveControlNoRepaint");
        return false;
    }

    for (uint64_t i = 0; i < controls.size(); i++)
        if (controls[i] == pControl)
        {
            controls.erase(controls.begin() + i);
            return true;
        }
    
    return false;
}

void Tether::Window::SetBackgroundColor(Color backgroundColor)
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetBackgroundColor");
        return;
    }

    this->backgroundColor = backgroundColor;
}

void Tether::Window::ClearWindow()
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetBackgroundColor");
        return;
    }
}

void Tether::Window::Repaint()
{
    if (!initialized)
    {
        DispatchNoInit("Window::SetBackgroundColor");
        return;
    }

    SpawnEvent(Events::EventType::WINDOW_REPAINT, 
    [this](Events::EventHandler* pEventHandler)
    {
        pEventHandler->OnWindowRepaint(Events::WindowRepaintEvent());
    });
    
    ClearWindow();
    
    for (uint64_t i = 0; i < controls.size(); i++)
        controls[i]->Render(this);
    
    SwapBuffers();
}

bool Tether::Window::InitGraphics()
{
    if (!initialized)
    {
        DispatchNoInit("Window::InitGraphics");
        return false;
    }

#ifdef OPENGL_API

#else
    throw "This version of Tether was compiled without OpenGL support";
#endif

    return true;
}

void Tether::Window::SwapBuffers()
{
    if (!initialized)
    {
        DispatchNoInit("Window::SwapBuffers");
        return;
    }
}

void Tether::Window::DisposeGraphics()
{
    if (!initialized)
    {
        DispatchNoInit("Window::DisposeGraphics");
        return;
    }
}
