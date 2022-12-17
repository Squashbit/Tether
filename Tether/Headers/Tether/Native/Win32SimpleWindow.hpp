#pragma once
#ifdef _WIN32

#include <Tether/Native/SimpleWindowNative.hpp>

#include <Windows.h> // oh no

namespace Tether::Native
{
	class TETHER_EXPORT Win32SimpleWindow : public SimpleWindowNative
	{
	public:
		Win32SimpleWindow(SimpleWindow* pWindow);

		void OnInit(int width, int height, const char* title, bool visible);

		bool Run();

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
		void SetButtonStyleBitmask(uint8_t style);
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

		int64_t HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void OnDispose();
	public:
		HWND window = 0;
		HINSTANCE hinst = nullptr;
	private:
		unsigned long CalculateStyle();
		void ReconstructStyle();

		RECT GetAdjustedRect(int x, int y, int width, int height);

		std::shared_ptr<wchar_t> ToWide(const char* str);
	private:
		std::string className = "";

		bool decorated = true;
		bool visible = false;
		bool resizable = true;
		bool closable = true;
		bool fullscreen = false;

		bool rawInputEnabled = false;
		bool rawInputInitialized = false;
		CursorMode cursorMode = CursorMode::NORMAL;

		bool boundsEnabled = false;
		int minWidth = 0; 
		int minHeight = 0;
		int maxWidth = 0;
		int maxHeight = 0;

		int setX = 0;
		int setY = 0;
		int setWidth = 0;
		int setHeight = 0;

		bool prevReceivedMouseMove = false;
		int mouseX = -1;
		int mouseY = -1;
		int relMouseX = -1;
		int relMouseY = -1;

		uint8_t styleMask = ButtonStyleMask::MAXIMIZE_BUTTON
			              | ButtonStyleMask::MINIMIZE_BUTTON;

		WNDCLASSEX wndClass;
	};
}

#endif //_WIN32