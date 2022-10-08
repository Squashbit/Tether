// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

#ifndef _TETHER_SIMPLEWINDOW_HPP
#define _TETHER_SIMPLEWINDOW_HPP

#include <Tether/Application.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/WindowHint.hpp>
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
	namespace Controls
	{
		class Control;
	}

	namespace Storage
	{
		struct VarStorage;
	}

#ifdef _WIN32
	class SimpleWindow;
	class TETHER_EXPORT WindowProcCaller
	{
	public:
		int64_t HandleMessage(void* hWnd, Tether::SimpleWindow* pWnd,
			uint32_t msg, uint64_t wParam, uint64_t lParam);
	};
#endif

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

	class TETHER_EXPORT SimpleWindow : public IDisposable
	{
	#ifdef _WIN32
		friend WindowProcCaller;
	#endif
		friend Tether::Controls::Control;
		friend Tether::Devices::DeviceManager;
	public:
		SimpleWindow();
		~SimpleWindow();
		TETHER_NO_COPY(SimpleWindow);

		void Hint(HintType type, int64_t pValue);

		/**
		 * @brief Initializes the window.
		 * 
		 * @param width The width of the window
		 * @param height The height of the window
		 * @param title The title of the window.
		 * 
		 * @returns True if the window was successfully created. 
		 *      Otherwise, false.
		 */
		bool Init(uint64_t width, uint64_t height, const char* title);

		/**
		 * @brief Called when the window is initialized.
		 */
		virtual void OnInit() {}
		
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

		void SetVisible(bool visibility);
		bool IsVisible();

		/**
		 * @brief Sets the window's fullscreen state.
		 * 
		 * @param fullscreen True if the window should be fullscreen;
		 *  otherwise, false.
		 * @param settings The settings to change the monitor to. 
		 * 	Note: Only works on Windows. On Linux, this parameter is ignored.
		 * @param monitor The monitor to go fullscreen on. 
		 * 	If this value is nullptr, fullscreen is on the first monitor.
		 * 	Tether must be compiled with TETHER_MONITORS to use this parameter.
		 */
		void SetFullscreen(
			bool fullscreen, 
			FullscreenSettings* settings = nullptr,
			Devices::Monitor* monitor = nullptr
		);

	#pragma region Cursor functions
		void SetRawInputEnabled(bool enabled);
		void SetCursorMode(CursorMode mode);
		void SetCursorPos(int x, int y);
		void SetCursorRootPos(int x, int y);
	#pragma endregion Cursor functions
		
	#pragma region GraphicalWindow setters
		void SetX(int64_t x);
		void SetY(int64_t y);
		void SetPosition(int64_t x, int64_t y);
		void SetWidth(uint64_t width);
		void SetHeight(uint64_t height);
		void SetSize(uint64_t width, uint64_t height);
		void SetTitle(const char* title);
		void SetDecorated(bool decorated);
		void SetClosable(bool closable);
		void SetResizable(bool resizable);
		void SetMinimizeBox(bool enabled);
		void SetMaximizeBox(bool enabled);
		void SetMaximized(bool maximized);
		void SetBoundsEnabled(bool enabled);
		void SetBounds(int64_t minWidth, int64_t minHeight, int64_t maxWidth,
			int64_t maxHeight);
	#pragma endregion GraphicalWindow Setters

	#pragma region GraphicalWindow getters
		// GraphicalWindow X
		int64_t GetX();
		// GraphicalWindow Y
		int64_t GetY();
		int64_t GetMouseX();
		int64_t GetMouseY();
		int64_t GetRelativeMouseX();
		int64_t GetRelativeMouseY();
		uint64_t GetWidth();
		uint64_t GetHeight();
	#pragma endregion GraphicalWindow getters
		
		/**
		 * @brief Processes all pending events for the window.
		 */
		void PollEvents();
		
	#ifdef __linux__
		void SetPreferredResizeInc(int width, int height);
	#endif

	#ifdef _WIN32
		unsigned long CalculateStyle();
		void ReconstructStyle();
	#endif

		Storage::VarStorage* GetStorage();

		void SetCloseRequested(bool requested);
		bool IsCloseRequested();
	protected:
		void SpawnEvent(
			Events::EventType eventType,
			std::function<void(Events::EventHandler*)> callEventFun
		);

		void SpawnInput(
			Input::InputType inputType,
			std::function<void(Input::InputListener*)> callInputFun
		);

		void DispatchNoInit(std::string functionName);
		void DispatchError(ErrorCode code, ErrorSeverity severity, 
			std::string functionName);
		void SpawnKeyInput(uint32_t scancode, uint32_t keycode, bool pressed);
		
	#ifdef __linux__
	#endif //__linux__
	#ifdef _WIN32
		int64_t HandleMessage(void* hWnd, uint32_t msg, uint32_t wParam, 
			uint64_t lParam);
	#endif // _WIN32

		Storage::VarStorage* storage = nullptr;
	private:
		bool LoadLibraries();

		void OnDispose();

	#ifdef __linux__
		void ProcessMwmFunctions();

		uint64_t prevX = 0, prevY = 0;
		uint64_t prevWidth = 0, prevHeight = 0;
	#endif

	#ifdef _WIN32
		std::shared_ptr<wchar_t> ToWide(const char* str);
		
		bool decorated = true;
		std::string className = "";
	#endif

		std::vector<WindowHint> hints;

		// GraphicalWindow stuff
		int64_t setX = 0;
		int64_t setY = 0;
		int64_t setWidth = 0;
		int64_t setHeight = 0;
		bool visible = false;
		bool fullscreen = false;
		bool closable = true;
		bool resizable = true;
		bool minimizeBox = true;
		bool maximizeBox = true;
		bool boundsEnabled = false;
		int64_t minWidth = 0, minHeight = 0, maxWidth = 0, maxHeight = 0;

		CursorMode cursorMode = CursorMode::NORMAL;

		// Mouse stuff
		bool prevReceivedMouseMove = false;
		bool rawInputEnabled = false;
		bool rawInputInitialized = false;
		int64_t mouseX = -1;
		int64_t mouseY = -1;
		int64_t relMouseX = -1;
		int64_t relMouseY = -1;

		// Each event has a list of handlers to handle that specific event.
		std::unordered_map<Events::EventType, 
			std::vector<Events::EventHandler*>> handlers;

		std::unordered_map<Input::InputType, 
			std::vector<Input::InputListener*>> inputListeners;

		Application* app;
		
		bool closeRequested = false;
	};
}

#endif //_TETHER_SIMPLEWINDOW_HPP