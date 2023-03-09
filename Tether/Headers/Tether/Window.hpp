#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Types.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Events/EventHandler.hpp>
#include <Tether/Events/EventType.hpp>
#include <Tether/Input/InputListener.hpp>
#include <Tether/Devices/Monitor.hpp>

#include <string_view>
#include <vector>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace Tether
{
	class TETHER_EXPORT Window
	{
	public:
		enum class Type
		{
			NORMAL
		};

		enum class CursorMode
		{
			NORMAL,
			HIDDEN,
			DISABLED,
		};

		static Scope<Window> Create(int width, int height, std::wstring_view title,
			bool visible = false);

		Window();
		virtual ~Window() = 0;
		TETHER_NO_COPY(Window);

		void AddEventHandler(Events::EventHandler& handler, 
			Events::EventType eventType);
		void RemoveEventHandler(Events::EventHandler& handler);
		void AddInputListener(Input::InputListener& listener, 
			Input::InputType inputType);
		void RemoveInputListener(Input::InputListener& listener);

		void SetCloseRequested(bool requested);
		bool IsCloseRequested() const;

		void SpawnEvent(
			Events::EventType eventType,
			std::function<void(Events::EventHandler&)> callEventFun
		);

		void SpawnInput(
			Input::InputType inputType,
			std::function<void(Input::InputListener&)> callInputFun
		);

		void SpawnKeyInput(uint32_t scancode, uint32_t keycode, bool pressed);

		virtual void SetVisible(bool visibility) = 0;
		virtual bool IsVisible() = 0;
		virtual void SetRawInputEnabled(bool enabled) = 0;
		virtual void SetCursorMode(Window::CursorMode mode) = 0;
		virtual void SetCursorPos(int x, int y) = 0;
		virtual void SetCursorRootPos(int x, int y) = 0;
		virtual void SetX(int x) = 0;
		virtual void SetY(int y) = 0;
		virtual void SetPosition(int x, int y) = 0;
		virtual void SetWidth(int width) = 0;
		virtual void SetHeight(int height) = 0;
		virtual void SetSize(int width, int height) = 0;
		virtual void SetTitle(std::wstring_view title) = 0;
		virtual void SetBoundsEnabled(bool enabled) = 0;
		virtual void SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight) = 0;
		virtual void SetDecorated(bool enabled) = 0;
		virtual void SetResizable(bool resizable) = 0;
		virtual void SetClosable(bool closable) = 0;
		virtual void SetButtonStyleBitmask(uint8_t mask) = 0;
		virtual void SetMaximized(bool maximized) = 0;
		virtual void SetPreferredResizeInc(int x, int y) = 0;
		virtual void SetFullscreen(bool fullscreen, const Devices::Monitor& monitor) = 0;
		virtual int GetX() = 0;
		virtual int GetY() = 0;
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;
		virtual int GetMouseX() = 0;
		virtual int GetMouseY() = 0;
		virtual int GetRelativeMouseX() = 0;
		virtual int GetRelativeMouseY() = 0;
		virtual bool IsFocused() = 0;
	protected:
		Application& m_App;
	private:
		std::atomic_bool m_CloseRequested = false;

		// Each event has a list of handlers to handle that specific event.
		std::shared_mutex m_HandlersMutex;
		std::unordered_map<Events::EventType, 
			std::vector<Events::EventHandler*>> m_Handlers;

		std::shared_mutex m_InputListenersMutex;
		std::unordered_map<Input::InputType, 
			std::vector<Input::InputListener*>> m_InputListeners;
	};
}
