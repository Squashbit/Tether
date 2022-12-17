/*
 * This file defines non platform specific fields of the GraphicalWindow class
 */

#include <Tether/SimpleWindow.hpp>
#include <Tether/Common/VectorUtils.hpp>
#include <Tether/Native/SimpleWindowNative.hpp>

#include <string.h>

using namespace Tether;
using namespace Storage;

SimpleWindow::SimpleWindow(int width, int height, const char* title, bool visible)
{
    app = &Application::Get();
    SetNative();

    native->OnInit(width, height, title, visible);
}

SimpleWindow::~SimpleWindow()
{
    Dispose();
    delete native;
}

bool SimpleWindow::Run()
{
    return native->Run();
}

void SimpleWindow::AddEventHandler(Events::EventHandler& handler, 
    Events::EventType eventType)
{
    AddEventHandler(&handler, eventType);
}

void SimpleWindow::AddEventHandler(Events::EventHandler* handler,
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

void SimpleWindow::RemoveEventHandler(Events::EventHandler& handler)
{
    RemoveEventHandler(&handler);
}

void SimpleWindow::RemoveEventHandler(Events::EventHandler* handler)
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

void SimpleWindow::AddInputListener(Input::InputListener& listener, 
	Input::InputType inputType)
{
    AddInputListener(&listener, inputType);
}

void SimpleWindow::AddInputListener(Input::InputListener* listener, 
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

void SimpleWindow::RemoveInputListener(Input::InputListener& listener)
{
    RemoveInputListener(&listener);
}

void SimpleWindow::RemoveInputListener(Input::InputListener* listener)
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

void SimpleWindow::SetVisible(bool visibility) 
{ native->SetVisible(visibility); }

void SimpleWindow::SetRawInputEnabled(bool enabled) 
{ native->SetRawInputEnabled(enabled); }

void SimpleWindow::SetCursorMode(CursorMode mode) 
{ native->SetCursorMode(mode); }

void SimpleWindow::SetCursorPos(int x, int y) 
{ native->SetCursorPos(x, y); }

void SimpleWindow::SetCursorRootPos(int x, int y) 
{ native->SetCursorRootPos(x, y); }

void SimpleWindow::SetX(int x) 
{ native->SetX(x); }

void SimpleWindow::SetY(int y) 
{ native->SetY(y); }

void SimpleWindow::SetPosition(int x, int y) 
{ native->SetPosition(x, y); }

void SimpleWindow::SetWidth(int width) 
{ native->SetWidth(width); }

void SimpleWindow::SetHeight(int height) 
{ native->SetHeight(height); }

void SimpleWindow::SetSize(int width, int height) 
{ native->SetSize(width, height); }

void SimpleWindow::SetTitle(const char* title) 
{ native->SetTitle(title); }

void SimpleWindow::SetBoundsEnabled(bool enabled) 
{ native->SetBoundsEnabled(enabled); }

void SimpleWindow::SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight) 
{ native->SetBounds(minWidth, minHeight, maxWidth, maxHeight); }

void SimpleWindow::SetDecorated(bool enabled) 
{ native->SetDecorated(enabled); }

void SimpleWindow::SetResizable(bool resizable) 
{ native->SetResizable(resizable); }

void SimpleWindow::SetClosable(bool closable) 
{ native->SetClosable(closable); }

void SimpleWindow::SetButtonStyleBitmask(uint8_t mask) 
{ native->SetButtonStyleBitmask(mask); }

void SimpleWindow::SetMaximized(bool maximized) 
{ native->SetMaximized(maximized); }

void SimpleWindow::SetFullscreen(bool fullscreen, FullscreenSettings* settings, 
    Devices::Monitor* monitor) 
{ native->SetFullscreen(fullscreen, settings, monitor); }

void SimpleWindow::PollEvents() 
{ native->PollEvents(); }

bool SimpleWindow::IsVisible() 
{ return native->IsVisible(); }

int SimpleWindow::GetX() 
{ return native->GetX(); }

int SimpleWindow::GetY() 
{ return native->GetY(); }

int SimpleWindow::GetWidth() 
{ return native->GetWidth(); }

int SimpleWindow::GetHeight() 
{ return native->GetHeight(); }

int SimpleWindow::GetMouseX() 
{ return native->GetMouseX(); }

int SimpleWindow::GetMouseY() 
{ return native->GetMouseY(); }

int SimpleWindow::GetRelativeMouseX() 
{ return native->GetRelativeMouseX(); }

int SimpleWindow::GetRelativeMouseY() 
{ return native->GetRelativeMouseY(); }

bool SimpleWindow::IsFocused()
{ return native->IsFocused(); }

void SimpleWindow::SetCloseRequested(bool requested)
{
    this->closeRequested = requested;
}

bool SimpleWindow::IsCloseRequested()
{
    return closeRequested;
}

void SimpleWindow::SpawnEvent(
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

void SimpleWindow::SpawnInput(
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

void SimpleWindow::SpawnKeyInput(uint32_t scancode, uint32_t keycode,
	bool pressed)
{
	Input::KeyInfo event(
		scancode,
		keycode,
		pressed
		);

	SpawnInput(Input::InputType::KEY,
		[&](Input::InputListener* pInputListener)
		{
			pInputListener->OnKey(event);
		});
}

Native::SimpleWindowNative* SimpleWindow::GetWindowNative()
{
    return native;
}

void SimpleWindow::OnDispose()
{
    native->OnDispose();
}
