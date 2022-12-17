#ifdef _WIN32

#include <Tether/Native/Win32SimpleWindow.hpp>

#include <Tether/Common/StringTools.hpp>
#include <Tether/Native.hpp>

#include <algorithm>
#include <cmath>
#include <string.h>

#undef ERROR

using namespace Tether::Storage;
using namespace Tether::Native;

static LRESULT CALLBACK Tether_WndProcRedir(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
{
	Win32SimpleWindow* pWindow = reinterpret_cast<Win32SimpleWindow*>(
		GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWindow->HandleMessage(hWnd, msg, wParam, lParam);
}

static LRESULT CALLBACK Tether_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
{
	// This window procedure is only to initialize the window user data before
	// using the actual redirection window procedure.
	
	if (msg == WM_NCCREATE)
	{
		CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Win32SimpleWindow* pWindow = reinterpret_cast<Win32SimpleWindow*>(
			pCreate->lpCreateParams);

		// Sanity check
		if (pWindow == nullptr)
			return 0;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		// Switch the window procedure
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, 
			reinterpret_cast<LONG_PTR>(Tether_WndProcRedir));
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Win32SimpleWindow::Win32SimpleWindow(SimpleWindow* pWindow)
	:
	SimpleWindowNative(pWindow)
{}

void Win32SimpleWindow::OnInit(int width, int height, const char* title, bool visible)
{
	// Grab the hinstance
	hinst = GetModuleHandle(NULL);
	
	this->setWidth = width;
	this->setHeight = height;
	
	RECT wr = GetAdjustedRect(0, 0, width, height);

	// Generate class name
	this->className = StringTools::RandomString(20);

	// Create class
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndClass.hIcon = LoadIcon(hinst, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(hinst, IDI_APPLICATION);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = className.c_str();
	wndClass.hInstance = hinst;
	wndClass.lpfnWndProc = Tether_WindowProc;

	RegisterClassEx(&wndClass);
	
	// Create window
	window = CreateWindowEx(
		0, // Extended style
		className.c_str(),
		title,
		CalculateStyle(),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left, // Width
		wr.bottom - wr.top, // Height
		nullptr, //Handle to the parent of this window
		nullptr, //Handle to the menu of child window identifier
		hinst,
		this
	);
	
	if (visible)
		pWindow->SetVisible(true);
}

bool Win32SimpleWindow::Run()
{
	MSG msg;
	while (GetMessage(&msg, window, 0, 0) > 0 && !pWindow->IsCloseRequested())
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void Win32SimpleWindow::SetVisible(bool visibility)
{
	if (visibility)
	{
		ShowWindow(window, SW_SHOW);
		UpdateWindow(window);
	}
	else
		ShowWindow(window, SW_HIDE);
	
	visible = visibility;
}

bool Win32SimpleWindow::IsVisible()
{
	return visible;
}

void Win32SimpleWindow::SetRawInputEnabled(bool enabled)
{
	if (enabled && !rawInputInitialized)
	{
		RAWINPUTDEVICE rawInputDevice;
		rawInputDevice.usUsagePage = 0x01; // Mouse
		rawInputDevice.usUsage = 0x02;
		rawInputDevice.dwFlags = 0;
		rawInputDevice.hwndTarget = window;

		if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
			return;

		rawInputInitialized = true;
	}

	rawInputEnabled = enabled;
}

void Win32SimpleWindow::SetCursorMode(CursorMode mode)
{
	switch (mode)
	{
		case CursorMode::NORMAL:
		{
			ShowCursor(true);
		}
		break;

		case CursorMode::HIDDEN:
		case CursorMode::DISABLED:
		{
			ShowCursor(false);
		}
		break;
	}

	cursorMode = mode;
}

static void SetCurPos(int x, int y)
{
	SetCursorPos(x, y);
}

void Win32SimpleWindow::SetCursorPos(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	ClientToScreen(window, &pt);
	SetCurPos(pt.x, pt.y);
}

void Win32SimpleWindow::SetCursorRootPos(int x, int y)
{
	SetCurPos(x, y);
}

void Win32SimpleWindow::SetX(int x)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		static_cast<int>(x), 
		wr.top, 
		wr.right - wr.left, 
		wr.bottom - wr.top, 
		false
	);

	this->setX = x;
}

void Win32SimpleWindow::SetY(int y)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		wr.left,
		static_cast<int>(y),
		wr.right - wr.left,
		wr.bottom - wr.top,
		false
	);

	this->setY = y;
}

void Win32SimpleWindow::SetPosition(int x, int y)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		static_cast<int>(x),
		static_cast<int>(y),
		wr.right - wr.left,
		wr.bottom - wr.top,
		false
	);

	this->setX = x;
	this->setY = y;
}

void Win32SimpleWindow::SetWidth(int width)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		wr.left,
		wr.top,
		width,
		wr.bottom - wr.top,
		false
	);

	this->setWidth = width;
}

void Win32SimpleWindow::SetHeight(int height)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		wr.left,
		wr.top,
		wr.right - wr.left,
		height,
		false
	);

	this->setHeight = height;
}

void Win32SimpleWindow::SetSize(int width, int height)
{
	RECT wr;
	GetWindowRect(window, &wr);

	MoveWindow(
		window,
		wr.left,
		wr.top,
		width,
		height,
		false
	);

	this->setWidth = width;
	this->setHeight = height;
}

void Win32SimpleWindow::SetTitle(const char* title)
{
	SetWindowTextA(window, title);
}

void Win32SimpleWindow::SetBoundsEnabled(bool enabled)
{
	boundsEnabled = enabled;
}

void Win32SimpleWindow::SetBounds(int minWidth, int minHeight,
	int maxWidth, int maxHeight)
{
	this->minWidth  = minWidth;
	this->minHeight = minHeight;
	this->maxWidth  = maxWidth;
	this->maxHeight = maxHeight;
}

void Win32SimpleWindow::SetDecorated(bool decorated)
{
	if (this->decorated == decorated)
		return;
	this->decorated = decorated;

	ReconstructStyle();
}

void Win32SimpleWindow::SetResizable(bool resizable)
{
	if (resizable == this->resizable)
		return;
	this->resizable = resizable;

	ReconstructStyle();
}

void Win32SimpleWindow::SetClosable(bool closable)
{
	if (closable == this->closable)
		return;
	this->closable = closable;
}

void Win32SimpleWindow::SetButtonStyleBitmask(uint8_t style)
{
	this->styleMask = style;
	ReconstructStyle();
}

void Win32SimpleWindow::SetMaximized(bool maximize)
{
	if (visible)
		if (maximize)
			ShowWindow(window, SW_MAXIMIZE);
		else
			ShowWindow(window, SW_SHOW);
}

void Win32SimpleWindow::SetFullscreen(
	bool fullscreen,
	FullscreenSettings* settings,
	Devices::Monitor* monitor
)
{
	if (this->fullscreen == fullscreen)
		return;

	if (fullscreen)
	{	
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = 1920;
		dmScreenSettings.dmPelsHeight = 1080;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
		SetWindowPos(window, HWND_TOP, 0, 0, dmScreenSettings.dmPelsWidth,
			dmScreenSettings.dmPelsHeight, SWP_SHOWWINDOW);
		ShowWindow(window, SW_MAXIMIZE);
	}
	else
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);

		ChangeDisplaySettings(&dmScreenSettings, CDS_RESET);

		ReconstructStyle();
		SetWindowPos(window, HWND_TOP, 
			static_cast<int>(setX), 
			static_cast<int>(setY), 
			static_cast<int>(setWidth), 
			static_cast<int>(setHeight),
			SWP_SHOWWINDOW
		);
		ShowWindow(window, SW_SHOW);
	}
	
	this->fullscreen = fullscreen;
}

int Win32SimpleWindow::GetX()
{
	RECT windowRect;
	GetWindowRect(window, (LPRECT)&windowRect);
	
	return windowRect.left;
}

int Win32SimpleWindow::GetY()
{
	RECT windowRect;
	GetWindowRect(window, (LPRECT)&windowRect);
	
	return windowRect.top;
}

int Win32SimpleWindow::GetWidth()
{
	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);
	
	return windowRect.right - windowRect.left;
}

int Win32SimpleWindow::GetHeight()
{
	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);

	return windowRect.bottom - windowRect.top;
}

int Win32SimpleWindow::GetMouseX()
{
	return mouseX;
}

int Win32SimpleWindow::GetMouseY()
{
	return mouseY;
}

int Win32SimpleWindow::GetRelativeMouseX()
{
	return relMouseX;
}

int Win32SimpleWindow::GetRelativeMouseY()
{
	return relMouseY;
}

bool Win32SimpleWindow::IsFocused()
{
	return GetForegroundWindow() == window;
}

void Win32SimpleWindow::PollEvents()
{
	if (!visible)
		return;

	MSG msg{};
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
		if (!IsWindow(window))
		{
			if (!closable)
				return;
			
			pWindow->SetCloseRequested(true);
			pWindow->SpawnEvent(Events::EventType::WINDOW_CLOSING,
			[this](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
			});
		}

	if (cursorMode == CursorMode::DISABLED && GetForegroundWindow() == window)
	{
		SetCursorPos(
			static_cast<int>(GetWidth() / 2),
			static_cast<int>(GetHeight() / 2)
		);
	}
}

void Win32SimpleWindow::OnDispose()
{
	DestroyWindow(window);
	UnregisterClass(this->className.c_str(), hinst);
}

std::shared_ptr<wchar_t> Win32SimpleWindow::ToWide(const char* str)
{
	// Size to allocate
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	
	WCHAR* wstr = new WCHAR[size];
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)wstr, size);

	// Allocate shared pointer
	return std::shared_ptr<wchar_t>(wstr, std::default_delete<wchar_t[]>());
}

unsigned long Win32SimpleWindow::CalculateStyle()
{
	LONG style = WS_OVERLAPPEDWINDOW | WS_SYSMENU;

	// TODO: Resize stuff

	if (!decorated)  style |= WS_POPUP;
	if (styleMask & ButtonStyleMask::MINIMIZE_BUTTON) style |= WS_MINIMIZEBOX;
	if (styleMask & ButtonStyleMask::MAXIMIZE_BUTTON) style |= WS_MAXIMIZEBOX;
	
	return style;
}

void Win32SimpleWindow::ReconstructStyle()
{
	SetWindowLongPtr(window, GWL_STYLE, CalculateStyle());
	UpdateWindow(window);
}

RECT Win32SimpleWindow::GetAdjustedRect(int x, int y, int width, int height)
{
	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, CalculateStyle(), false);

	return wr;
}

int64_t Win32SimpleWindow::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
{
	using namespace Tether::Events;

	// The window variable should never be used in this function because
	// the CreateWindow function sends events before it has finished execution;
	// therefore, it is possible for the window variable to be null. 
	// Use hWnd instead. Why does WinAPI do this? I have no idea.

	uint32_t wParam32 = (uint32_t)wParam;

	switch (msg)
	{
		case WM_CLOSE:
		{
			if (!closable)
				break;

			pWindow->SetCloseRequested(true);
			pWindow->SpawnEvent(Events::EventType::WINDOW_CLOSING,
			[this](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
			});

			return 0;
		}
		break;

		case WM_KEYDOWN:
		{
			if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
				return 0;

			// Windows does this thing where you can't distinguish between either 
			// shift keys, so instead, just press both and hope for the best.
			if (wParam == VK_SHIFT)
			{
				UINT scancode = MapVirtualKeyA(wParam32, MAPVK_VK_TO_VSC);
				pWindow->SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, true);
				pWindow->SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, true);

				return 0;
			}

			pWindow->SpawnKeyInput(MapVirtualKeyA(wParam32, MAPVK_VK_TO_VSC), wParam32,
				true);

			return 0;
		}
		break;

		case WM_KEYUP:
		{
			if (wParam == VK_SHIFT)
			{
				UINT scancode = MapVirtualKeyA(wParam32, MAPVK_VK_TO_VSC);
				pWindow->SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, false);
				pWindow->SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, false);

				return 0;
			}

			pWindow->SpawnKeyInput(MapVirtualKeyA(wParam32, MAPVK_VK_TO_VSC), wParam32,
				false);

			return 0;
		}
		break;

		case WM_CHAR:
		{
			Input::KeyCharInfo event(
				(char)wParam,
				(HIWORD(lParam) & KF_REPEAT) == KF_REPEAT
			);

			pWindow->SpawnInput(Input::InputType::KEY_CHAR,
				[&](Input::InputListener* pInputListener)
			{
				pInputListener->OnKeyChar(event);
			});
		}
		break;

		case WM_MOUSEMOVE:
		{
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);

			POINT mouse;
			mouse.x = x;
			mouse.y = y;

			ClientToScreen(window, &mouse);

			if (x == relMouseX && y == relMouseY)
				break;

			if (!prevReceivedMouseMove)
			{
				mouseX = mouse.x;
				mouseY = mouse.y;
				relMouseX = x;
				relMouseY = y;
			}

			Input::MouseMoveInfo event(
				mouse.x,
				mouse.y,
				x,
				y,
				relMouseX,
				relMouseY,
				mouseX,
				mouseY
			);

			pWindow->SpawnInput(Input::InputType::MOUSE_MOVE,
			[&](Input::InputListener* pInputListener)
			{
				pInputListener->OnMouseMove(event);
			});

			mouseX = mouse.x;
			mouseY = mouse.y;
			relMouseX = x;
			relMouseY = y;

			prevReceivedMouseMove = true;
		}
		break;

		case WM_MOVE:
		{
			WindowMoveEvent event(
				GetX(),
				GetY()
			);

			pWindow->SpawnEvent(Events::EventType::WINDOW_MOVE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowMove(event);
			});
		}
		break;

		case WM_INPUT:
		{
			UINT dataSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL,
				&dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());

				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
					rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize
					&& raw->header.dwType == RIM_TYPEMOUSE)
				{
					Input::RawMouseMoveInfo event(
						raw->data.mouse.lLastX,
						raw->data.mouse.lLastY
					);

					pWindow->SpawnInput(Input::InputType::RAW_MOUSE_MOVE,
						[&](Input::InputListener* pInputListener)
					{
						pInputListener->OnRawMouseMove(event);
					});
				}
			}

			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;

		case WM_SIZE:
		{
			if (wParam != SIZE_RESTORED && wParam != SIZE_MAXIMIZED)
				break;

			int width = LOWORD(lParam);
			int height = HIWORD(lParam);

			if (width == 0 && height == 0)
				break;

			WindowResizeEvent event(
				width,
				height
			);

			pWindow->SpawnEvent(Events::EventType::WINDOW_RESIZE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowResize(event);
			});
		}
		break;

		case WM_GETMINMAXINFO:
		{
			if (boundsEnabled)
			{
				MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
				pInfo->ptMinTrackSize = { (LONG)minWidth, (LONG)minHeight };
				pInfo->ptMaxTrackSize = { (LONG)maxWidth, (LONG)maxHeight };
			}
		}
		break;

		default:
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	}

	return 0;
}

#endif //_WIN32