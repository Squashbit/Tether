#ifndef _TETHER_IWINDOW_HPP
#define _TETHER_IWINDOW_HPP

#include <Tether/Common/Color.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/WindowHint.hpp>
#include <Tether/Events/EventHandler.hpp>
#include <Tether/Events/EventType.hpp>

#include <vector>
#include <functional>

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif //__linux__

#ifdef _WIN32
#include <Windows.h>
#endif //__linux__

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

#ifdef _WIN32
	class IWindow;
	class WindowProcCaller
	{
	public:
		LRESULT HandleMessage(HWND hWnd, Tether::IWindow* pWnd,
			UINT msg, WPARAM wParam, LPARAM lParam);
	};
#endif

	enum class FullscreenFields
	{
		WIDTH,
		HEIGHT,
		BITSPERPIXEL
	};

	struct FullscreenSettings
	{
		uint64_t width = 0;
		uint64_t height = 0;
		uint64_t bitsPerPixel = 0;
		uint64_t fields = 0;
	};

	class IWindow : public IDisposable
	{
	#ifdef _WIN32
		friend WindowProcCaller;
	#endif
		friend Tether::Controls::Control;
	public:
		IWindow() {}
		
		IWindow(const IWindow&) = delete;
		IWindow(IWindow&&) = delete;
		IWindow& operator=(const IWindow&) = delete;
		IWindow& operator=(IWindow&&) = delete;

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

		void SetVisible(bool visibility);
		bool IsVisible();

		void SetCursorVisible(bool show);
		void SetMousePos(uint64_t x, uint64_t y);
		void SetMouseRootPos(uint64_t x, uint64_t y);

		/**
		 * @brief Sets the window's fullscreen state.
		 * 
		 * @param fullscreen True if the window should be fullscreen;
		 *  otherwise, false.
		 * @param monitor The monitor index to fullscreen on. Obsolete if
		 *  fullscreen is false.
		 */
		void SetFullscreen(bool fullscreen, int monitor = 0);

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
		// Window X
		int64_t GetX();
		// Window Y
		int64_t GetY();
		uint64_t GetMouseX();
		uint64_t GetMouseY();
		uint64_t GetRelativeMouseX();
		uint64_t GetRelativeMouseY();
		uint64_t GetWidth();
		uint64_t GetHeight();
		
		/**
		 * @brief Processes all pending events for the window.
		 */
		void PollEvents();
		
	#ifdef __linux__
		void SetPreferredResizeInc(int width, int height);

		Display* GetDisplay();
		int GetScreen();
		uint64_t GetHandle();
	#endif

	#ifdef _WIN32
		HINSTANCE GetHIstance();
		HWND GetHandle();
	#endif

		bool IsCloseRequested();
		void IgnoreClose();
	protected:
		void SpawnEvent(
			Events::EventType eventType,
			std::function<void(Events::EventHandler*)> callEventFun
		);

		void DispatchNoInit(std::string functionName);

	#ifdef __linux__
		unsigned long window = 0;
		Display* display = nullptr;
		int screen = 0;

		XEvent event;
	#endif //__linux__
	#ifdef _WIN32
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND window = 0;
		// There is only ever one hinstance but for simplicity it is stored in the
		// window.
		HINSTANCE hinst = nullptr;

		WNDCLASSEX wndClass;
	#endif // _WIN32
	private:
		void OnDispose();

	#ifdef __linux__
		void ProcessMwmFunctions();
	#endif

	#ifdef _WIN32
		std::shared_ptr<wchar_t> ToWide(const char* str);
		RECT GetAdjustedRect();
		void ReconstructStyle();
		
		int64_t minWidth = 0, minHeight = 0, maxWidth = 0, maxHeight = 0;
		bool decorated = true;
		std::string className = "";
	#endif

		std::vector<WindowHint> hints;

		// Window stuff
		int64_t x = 200;
		int64_t y = 200;
		uint64_t width = 800;
		uint64_t height = 600;
		bool visible = false;
		bool fullscreen = false;
		bool closable = true;
		bool resizable = true;
		bool boundsEnabled = false;

		// Mouse stuff
		bool prevReceivedMouseMove = false;
		uint64_t mouseX = 0;
		uint64_t mouseY = 0;
		uint64_t relMouseX = 0;
		uint64_t relMouseY = 0;

		// Each event has a list of handlers to handle that specific event.
		std::unordered_map<Events::EventType, 
			std::vector<Events::EventHandler*>> handlers;
		
		bool closeRequested = false;
	};
}

#endif //_TETHER_IWINDOW_HPP