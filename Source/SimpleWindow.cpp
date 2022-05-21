/*
 * This file defines non platform specific fields of the GraphicalWindow class
 */

#include <Tether/SimpleWindow.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/Native.hpp>

#include <Tether/Controls/Control.hpp>

#include <string.h>

using namespace Tether;
using namespace Tether::Storage;

Tether::SimpleWindow::SimpleWindow()
{
    storage = new VarStorage();
}

Tether::SimpleWindow::~SimpleWindow()
{
    delete storage;
}

VarStorage* Tether::SimpleWindow::GetStorage()
{
	return storage;
}

void Tether::SimpleWindow::Hint(HintType type, int64_t pValue)
{
    // Hints can only be set before the window is initialized.
    if (!initialized)
        hints.push_back({ type, pValue });
}

void Tether::SimpleWindow::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void Tether::SimpleWindow::AddEventHandler(Events::EventHandler* handler,
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
    handler->OnAdd(this);
}

void Tether::SimpleWindow::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void Tether::SimpleWindow::RemoveEventHandler(Events::EventHandler* handler)
{
    handler->OnRemove(this);

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

void Tether::SimpleWindow::AddInputListener(Input::InputListener& listener, 
	Input::InputType inputType)
{
    AddInputListener(&listener, inputType);
}

void Tether::SimpleWindow::AddInputListener(Input::InputListener* listener, 
	Input::InputType inputType)
{
    using namespace Input;

    // Check if there are any event handlers of this type and create a vector
    // for that event type if there aren't any.
    if (inputListeners.count(inputType) == 0)
        inputListeners[inputType] = std::vector<Input::InputListener*>();
    
    // If the event handler is already in the vector, don't add it and return.
    if (VectorUtils::Contains(inputListeners[inputType], listener))
        return;
    
    // At this point, we know the vector of events exists and that this
    // handler isn't in that vector, so we are safe to add it.
    inputListeners[inputType].push_back(listener);
    listener->OnAdd(this);
}

void Tether::SimpleWindow::RemoveInputListener(Input::InputListener& listener)
{
    RemoveInputListener(&listener);
}

void Tether::SimpleWindow::RemoveInputListener(Input::InputListener* listener)
{
    listener->OnRemove(this);

    std::vector<Input::InputType> toErase;

    for (auto iter = inputListeners.begin(); iter != inputListeners.end(); iter++)
    {
        VectorUtils::EraseAll(iter->second, listener);

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
        inputListeners.erase(inputListeners.find(toErase[i]));
}

int64_t Tether::SimpleWindow::GetMouseX()
{
    return mouseX;
}

int64_t Tether::SimpleWindow::GetMouseY()
{
    return mouseY;
}

int64_t Tether::SimpleWindow::GetRelativeMouseX()
{
    return relMouseX;
}

int64_t Tether::SimpleWindow::GetRelativeMouseY()
{
    return relMouseY;
}

void Tether::SimpleWindow::SetCloseRequested(bool requested)
{
    this->closeRequested = requested;
}

bool Tether::SimpleWindow::IsCloseRequested()
{
    return closeRequested;
}

void Tether::SimpleWindow::SpawnEvent(
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

void Tether::SimpleWindow::SpawnInput(
    Input::InputType inputType,
	std::function<void(Input::InputListener*)> callInputFun
)
{
    if (inputListeners.count(inputType))
    {
        std::vector<Input::InputListener*> listenerList = inputListeners[inputType];
        for (uint64_t i = 0; i < listenerList.size(); i++)
            callInputFun(listenerList[i]);
    }
}

void Tether::SimpleWindow::DispatchNoInit(std::string functionName)
{
    SpawnEvent(Events::EventType::WINDOW_ERROR, 
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

void Tether::SimpleWindow::DispatchError(ErrorCode code, ErrorSeverity severity, 
    std::string functionName)
{
    SpawnEvent(Events::EventType::WINDOW_ERROR, 
    [&](Events::EventHandler* pEventHandler)
    {
        Events::WindowErrorEvent error(
            code,
            severity,
            functionName
        );

        pEventHandler->OnWindowError(error);
    });
}
