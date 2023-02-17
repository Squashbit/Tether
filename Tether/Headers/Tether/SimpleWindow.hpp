#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Types.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Events/EventHandler.hpp>
#include <Tether/Events/EventType.hpp>
#include <Tether/Input/InputListener.hpp>
#include <Tether/Devices/DeviceManager.hpp>
#include <Tether/Devices/Monitor.hpp>

#include <vector>
#include <functional>

#define TETHER_ASSERT_INITIALIZED(funcName) \
	if (!initialized) \
	{ \
		DispatchNoInit(funcName); \
		return; \
	}

#define TETHER_ASSERT_INITIALIZED_RET(funcName, returnValue) \
	if (!initialized) \
	{ \
		DispatchNoInit(funcName); \
		return returnValue; \
	}

namespace Tether
{
	namespace Native
	{
		class SimpleWindowNative;
	}

	namespace Controls
	{
		class Control;
	}

	// Note: Only works on Windows
	enum class FullscreenFields
	{
		WIDTH        = 1,
		HEIGHT       = 1 << 1,
		BITSPERPIXEL = 1 << 2
	};

	// Note: Only works on Windows
	struct FullscreenSettings
	{
		uint64_t width = 0;
		uint64_t height = 0;
		uint64_t bitsPerPixel = 0;
		uint64_t fields = 0;
	};

	struct ButtonStyleMask
	{
		static constexpr const uint8_t MINIMIZE_BUTTON = 1 << 0;
		static constexpr const uint8_t MAXIMIZE_BUTTON = 1 << 1;
	};

	class TETHER_EXPORT SimpleWindow : public IDisposable
	{
		friend Tether::Controls::Control;
		friend Tether::Devices::DeviceManager;
	public:
		SimpleWindow(int width, int height, const char* title, bool visible = false);
		~SimpleWindow();
		TETHER_NO_COPY(SimpleWindow);

		bool Run();

		void AddEventHandler(Events::EventHandler& handler, 
			Events::EventType eventType);
		void AddEventHandler(Events::EventHandler* handler, 
			Events::EventType eventType);
		void RemoveEventHandler(Events::EventHandler& handler);
		void RemoveEventHandler(Events::EventHandler* handler);
		void AddInputListener(Input::InputListener& listener, 
			Input::InputType inputType);
		void AddInputListener(Input::InputListener* listener, 
			Input::InputType inputType);
		void RemoveInputListener(Input::InputListener& listener);
		void RemoveInputListener(Input::InputListener* listener);

		bool IsVisible();
		void SetVisible(bool visibility);
		void SetRawInputEnabled(bool enabled);
		void SetCursorMode(CursorMode mode);
		void SetCursorPos(int x, int y);
		void SetCursorRootPos(int x, int y);
		void SetX(int x);
		void SetY(int y);
		void SetPosition(int x, int y);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetSize(int width, int height);
		void SetTitle(const char* title);
		void SetBoundsEnabled(bool enabled);
		void SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight);
		void SetDecorated(bool enabled);
		void SetResizable(bool resizable);
		void SetClosable(bool closable);
		void SetButtonStyleBitmask(uint8_t mask);
		void SetMaximized(bool maximized);
		void SetFullscreen(bool fullscreen, FullscreenSettings* settings,
			Devices::Monitor* monitor);
		void PollEvents();
		int GetX();
		int GetY();
		int GetWidth();
		int GetHeight();
		int GetMouseX();
		int GetMouseY();
		int GetRelativeMouseX();
		int GetRelativeMouseY();
		bool IsFocused();

		void SetCloseRequested(bool requested);
		bool IsCloseRequested();

		void SpawnEvent(
			Events::EventType eventType,
			std::function<void(Events::EventHandler*)> callEventFun
		);

		void SpawnInput(
			Input::InputType inputType,
			std::function<void(Input::InputListener*)> callInputFun
		);

		void SpawnKeyInput(uint32_t scancode, uint32_t keycode, bool pressed);

		virtual void OnPollEvent() {}

		Native::SimpleWindowNative* GetWindowNative();
	private:
		void SetNative();

		void OnDispose();

		bool closeRequested = false;

		// Each event has a list of handlers to handle that specific event.
		std::unordered_map<Events::EventType, 
			std::vector<Events::EventHandler*>> handlers;

		std::unordered_map<Input::InputType, 
			std::vector<Input::InputListener*>> inputListeners;

		Native::SimpleWindowNative* native = nullptr;
		Application* app;
	};
}
