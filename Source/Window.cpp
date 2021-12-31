/*
 * This file defines non platform specific fields of the Window class
 */

#include <Tether/Window.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;

static std::string defaultVertexSource = R"(
#version 450

layout (location = 0) in vec2 pos;

uniform vec2 windowSize;

void main()
{
    vec2 finalPos = pos;
    finalPos.x /= windowSize.x;

    gl_Position = vec4(finalPos.xy, 0, 0);
}
)";

static std::string defaultFragmentSource = R"(
#version 450

out vec4 fragColor;

void main()
{
    fragColor = vec4(1, 1, 1, 1);
}
)";

void Tether::Window::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void Tether::Window::AddEventHandler(Events::EventHandler* handler,
    Events::EventType eventType)
{
    using namespace Events;

    // Check if there are any event handlers of this type and create a vector
    // for that event type if there aren't any.
    if (handlers.count(eventType) == 0)
        handlers[eventType] = std::vector<EventHandler*>();
    
    // If the event handler is already in the vector, don't add it and return.
    if (VectorUtils::Contains(handlers[eventType], handler))
        return;
    
    // At this point, we know the vector of events exists and that this
    // handler isn't in that vector, so we are safe to add it.
    handlers[eventType].push_back(handler);
}

void Tether::Window::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void Tether::Window::RemoveEventHandler(Events::EventHandler* handler)
{
    std::vector<Events::EventType> toErase;

    for (auto iter = handlers.begin(); iter != handlers.end(); iter++)
    {
        VectorUtils::EraseAll(iter->second, handler);

        if (iter->second.size() == 0)
        {
            // We cannot erase anything from handlers right here
            // since that would be the same as a concurrent modification
            // exception in Java since we are actively looping through the
            // handlers.
            toErase.push_back(iter->first);
        }
    }

    for (uint64_t i = 0; i < toErase.size(); i++)
        handlers.erase(handlers.find(toErase[i]));
}

uint64_t Tether::Window::GetMouseX()
{
    return mouseX;
}

uint64_t Tether::Window::GetMouseY()
{
    return mouseY;
}

uint64_t Tether::Window::GetRelativeMouseX()
{
    return relMouseX;
}

uint64_t Tether::Window::GetRelativeMouseY()
{
    return relMouseY;
}

uint64_t Tether::Window::GetWidth()
{
    return width;
}

uint64_t Tether::Window::GetHeight()
{
    return height;
}

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

bool Tether::Window::IsCloseRequested()
{
    return closeRequested;
}

void Tether::Window::IgnoreClose()
{
    closeRequested = false;
}

void Tether::Window::ClearWindow()
{
    if (!initialized || stripped)
        return;
    
}

void Tether::Window::Repaint()
{
    if (!initialized || stripped)
        return;

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
    return true;
}

void Tether::Window::SpawnEvent(
    Events::EventType eventType,
    std::function<void(Events::EventHandler*)> callEventFun
)
{
    if (handlers.count(eventType))
    {
        std::vector<Events::EventHandler*> eventList = handlers[eventType];
        for (uint64_t i = 0; i < eventList.size(); i++)
            callEventFun(eventList[i]);
    }
}

void Tether::Window::DispatchNoInit(std::string functionName)
{
    SpawnEvent(Events::EventType::WINDOW_CLOSING, 
    [&](Events::EventHandler* pEventHandler)
    {
        Events::WindowErrorEvent error(
            ErrorCode::NOT_INITIALIZED,
            ErrorSeverity::LOW,
            functionName
        );

        pEventHandler->OnWindowError(error);
    });
}

void Tether::Window::DisposeGraphics()
{
    
}
