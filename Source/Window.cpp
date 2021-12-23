/*
 * This file defines non platform specific fields of the Window class
 */

#include <Link/Window.hpp>
#include <Link/Common/VectorUtils.hpp>
#include <Link/Controls/Control.hpp>

using namespace Link;

void Link::Window::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void Link::Window::AddEventHandler(Events::EventHandler* handler,
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

void Link::Window::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void Link::Window::RemoveEventHandler(Events::EventHandler* handler)
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

void Link::Window::ClearWindow()
{
    if (!initialized || stripped)
        return;
    
    glClearColor(
        backgroundColor.GetR(),
        backgroundColor.GetG(),
        backgroundColor.GetB(),
        1.0f
    );
    
    glClear(GL_COLOR_BUFFER_BIT);
}

void Link::Window::Repaint()
{
    if (!initialized || stripped)
        return;

    glViewport(GetX(), GetY(), width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -10000, 10000);

    ClearWindow();
    
    for (uint64_t i = 0; i < controls.size(); i++)
        controls[i]->Render(this);
    
    SwapBuffers();
}

void Link::Window::InitGraphics()
{
    float rectVertices[] = 
    {
        0, 1, 1, 0
    };

    rectVAO.Init();

    rectVertexBuffer.Init(&rectVAO);
    rectVertexBuffer.SetTarget(GL_ARRAY_BUFFER);
    rectVertexBuffer.BufferData(sizeof(rectVertices), rectVertices, 
        GL_STATIC_DRAW);
    rectVertexBuffer.VertexAttribPointer(0, 2, GL_FLOAT, false, 
        sizeof(float) * 2, (void*)0);
}
