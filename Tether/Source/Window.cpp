#include <Tether/Window.hpp>
#include <Tether/Common/VectorUtils.hpp>

#include <cstring>

namespace Tether
{
	Window::Window()
		:
		m_App(Application::Get())
	{}

	Window::~Window() {}

	void Window::AddEventHandler(Events::EventHandler& handler,
		Events::EventType eventType)
	{
		using namespace Events;

		std::scoped_lock handlersLock(m_HandlersMutex);

		std::vector<EventHandler*>& eventTypeHandlers = m_Handlers[eventType];
		for (size_t i = 0; i < eventTypeHandlers.size(); i++)
			if (eventTypeHandlers[i] == &handler)
				return;

		m_Handlers[eventType].push_back(&handler);
	}

	void Window::RemoveEventHandler(Events::EventHandler& handler)
	{
		using namespace Events;

		std::scoped_lock handlersLock(m_HandlersMutex);

		std::vector<Events::EventType> toErase;

		for (auto iter = m_Handlers.begin(); iter != m_Handlers.end(); iter++)
		{
			std::vector<EventHandler*>& eventTypeHandlers = iter->second;

			for (size_t i = 0; i < eventTypeHandlers.size(); i++)
				if (eventTypeHandlers[i] == &handler)
				{
					eventTypeHandlers.erase(eventTypeHandlers.begin() + i);
					break;
				}

			if (eventTypeHandlers.size() == 0)
				toErase.push_back(iter->first);
		}

		for (size_t i = 0; i < toErase.size(); i++)
			m_Handlers.erase(m_Handlers.find(toErase[i]));
	}

	void Window::AddInputListener(Input::InputListener& listener,
		Input::InputType inputType)
	{
		using namespace Input;

		std::scoped_lock listenersLock(m_InputListenersMutex);

		std::vector<InputListener*>& inputTypeListeners = m_InputListeners[inputType];
		for (size_t i = 0; i < inputTypeListeners.size(); i++)
			if (inputTypeListeners[i] == &listener)
				return;

		m_InputListeners[inputType].push_back(&listener);
	}

	void Window::RemoveInputListener(Input::InputListener& listener)
	{
		using namespace Input;

		std::scoped_lock listenersLock(m_InputListenersMutex);

		std::vector<InputType> toErase;

		for (auto iter = m_InputListeners.begin(); iter != m_InputListeners.end(); 
			iter++)
		{
			std::vector<InputListener*>& inputTypeListeners = iter->second;

			for (size_t i = 0; i < inputTypeListeners.size(); i++)
				if (inputTypeListeners[i] == &listener)
				{
					inputTypeListeners.erase(inputTypeListeners.begin() + i);
					break;
				}

			if (inputTypeListeners.size() == 0)
				toErase.push_back(iter->first);
		}

		for (size_t i = 0; i < toErase.size(); i++)
			m_InputListeners.erase(m_InputListeners.find(toErase[i]));
	}

	void Window::SetCloseRequested(bool requested)
	{
		this->m_CloseRequested = requested;
	}

	bool Window::IsCloseRequested()
	{
		return m_CloseRequested;
	}

	void Window::SpawnEvent(
		Events::EventType eventType,
		std::function<void(Events::EventHandler*)> callEventFun
	)
	{
		std::shared_lock handlersLock(m_HandlersMutex);

		if (!m_Handlers.count(eventType))
			return;

		std::vector<Events::EventHandler*> eventList = m_Handlers[eventType];
		for (uint64_t i = 0; i < eventList.size(); i++)
			callEventFun(eventList[i]);
	}

	void Window::SpawnInput(
		Input::InputType inputType,
		std::function<void(Input::InputListener&)> callInputFun
	)
	{
		std::shared_lock listenersLock(m_InputListenersMutex);

		if (!m_InputListeners.count(inputType))
			return;

		std::vector<Input::InputListener*> listenerList = m_InputListeners[inputType];
		for (uint64_t i = 0; i < listenerList.size(); i++)
			callInputFun(*listenerList[i]);
	}

	void Window::SpawnKeyInput(uint32_t scancode, uint32_t keycode,
		bool pressed)
	{
		Input::KeyInfo event(
			scancode,
			keycode,
			pressed
		);

		SpawnInput(Input::InputType::KEY,
			[&](Input::InputListener& inputListener)
		{
			inputListener.OnKey(event);
		});
	}
}
