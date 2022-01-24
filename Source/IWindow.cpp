/*
 * This file defines non platform specific fields of the Window class
 */

#include <Tether/IWindow.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/Controls/Control.hpp>

using namespace Tether;

void Tether::IWindow::Hint(HintType type, int64_t pValue)
{
    // Hints can only be set before the window is initialized.
    if (!initialized)
        hints.push_back({ type, pValue });
}

void Tether::IWindow::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void Tether::IWindow::AddEventHandler(Events::EventHandler* handler,
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

void Tether::IWindow::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void Tether::IWindow::RemoveEventHandler(Events::EventHandler* handler)
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

uint64_t Tether::IWindow::GetMouseX()
{
    return mouseX;
}

uint64_t Tether::IWindow::GetMouseY()
{
    return mouseY;
}

uint64_t Tether::IWindow::GetRelativeMouseX()
{
    return relMouseX;
}

uint64_t Tether::IWindow::GetRelativeMouseY()
{
    return relMouseY;
}

uint64_t Tether::IWindow::GetWidth()
{
    return width;
}

uint64_t Tether::IWindow::GetHeight()
{
    return height;
}

bool Tether::IWindow::IsCloseRequested()
{
    return closeRequested;
}

void Tether::IWindow::IgnoreClose()
{
    closeRequested = false;
}

void Tether::IWindow::SpawnEvent(
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

void Tether::IWindow::DispatchNoInit(std::string functionName)
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
