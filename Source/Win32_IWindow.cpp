#ifdef _WIN32

#include <Tether/IWindow.hpp>
#include <Tether/Controls/Control.hpp>
#include <Tether/Common/StringUtils.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

#define TETHER_WINSTYLE() GetWindowLong(window, GWL_STYLE)

LRESULT Tether::WindowProcCaller::HandleMessage(HWND hWnd, Tether::IWindow* pWnd,
	UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (pWnd->window == NULL)
		pWnd->window = hWnd;

	return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
}

static Tether::WindowProcCaller caller;

LRESULT CALLBACK Tether_WndProcRedir(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Tether::IWindow* pWindow = reinterpret_cast<Tether::IWindow*>(
		GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return caller.HandleMessage(hWnd, pWindow, msg, wParam, lParam);
}

LRESULT CALLBACK Tether_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// This window proc is only to initialize the window userdata before
	// using the actual redirection window proc.
	
	if (msg == WM_NCCREATE)
	{
		CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Tether::IWindow* pWindow = reinterpret_cast<Tether::IWindow*>(
			pCreate->lpCreateParams);

		// Sanity check
		if (pWindow == nullptr)
			return 0;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		// Switch the window proc
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, 
			reinterpret_cast<LONG_PTR>(Tether_WndProcRedir));
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Tether::IWindow::Init(uint64_t width, uint64_t height, const char* title)
{
	// Check if initialized
	// Not possible to use TETHER_ASSERT_INITIALIZED here
	if (initialized)
	{
		DispatchNoInit("IWindow::Init");
		return false;
	}

	// Grab the hinstance
	hinst = GetModuleHandle(NULL);
	if (!hinst)
		return false;
	
	this->width = width;
	this->height = height;
	this->closeRequested = false;
	
	bool shouldShow = true;
	bool fullscreenMode = false;
	for (uint64_t i = 0; i < hints.size(); i++)
	{
		int64_t value = hints[i].value;
		switch (hints[i].type)
		{
			case HintType::X: this->x = value; break;
			case HintType::Y: this->y = value; break;
			case HintType::VISIBLE: shouldShow = (bool)value; break;
			case HintType::FULLSCREEN: fullscreenMode = (bool)value; break;
		}
	}

	RECT wr = GetAdjustedRect();

	// Generate class name
	this->className = StringUtils::RandomString(20);

	// Create class
	this->wndClass.cbSize = sizeof(WNDCLASSEX);
	this->wndClass.style = CS_HREDRAW | CS_VREDRAW;
	this->wndClass.cbClsExtra = 0;
	this->wndClass.cbWndExtra = 0;
	this->wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	this->wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	this->wndClass.hIcon = LoadIcon(hinst, IDI_APPLICATION);
	this->wndClass.hIconSm = LoadIcon(hinst, IDI_APPLICATION);
	this->wndClass.lpszMenuName = nullptr;
	this->wndClass.lpszClassName = className.c_str();
	this->wndClass.hInstance = hinst;
	this->wndClass.lpfnWndProc = Tether_WindowProc;

	if (!RegisterClassEx(&this->wndClass))
		return false;
	
	// Create window
	window = CreateWindowEx(
		0, // Extended style
		className.c_str(),
		title,
		WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU,
		wr.left,
		wr.top,
		wr.right - wr.left, // Width
		wr.bottom - wr.top, // Height
		nullptr, //Handle to the parent of this window
		nullptr, //Handle to the menu of child window identifier
		hinst,
		this
	);
	if (!window)
	{
		closeRequested = true;
		return false;
	}
	
	initialized = true;

	if (fullscreenMode)
	{
		// TODO: Monitor window hint
		SetFullscreen(true, 0);
	}

	if (shouldShow)
		SetVisible(true);
	
	OnInit();

	return true;
}

void Tether::IWindow::SetVisible(bool visibility)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetVisible");

	if (visibility)
	{
		ShowWindow(window, SW_SHOW);
		UpdateWindow(window);
	}
	else
		ShowWindow(window, SW_HIDE);
	
	visible = visibility;
}

bool Tether::IWindow::IsVisible()
{
	return visible && initialized;
}

void Tether::IWindow::SetCursorVisible(bool show)
{
	ShowCursor(show);
}

void Tether::IWindow::SetMousePos(uint64_t x, uint64_t y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	ClientToScreen(window, &pt);
	SetCursorPos(pt.x, pt.y);
}

void Tether::IWindow::SetMouseRootPos(uint64_t x, uint64_t y)
{
	SetCursorPos(x, y);
}

void Tether::IWindow::SetX(int64_t x)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetX");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetY(int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetY");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetPosition(int64_t x, int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetPosition");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetWidth(uint64_t width)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetWidth");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetHeight(uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetHeight");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetSize");

	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetTitle(const char* title)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetTitle");

	SetWindowTextA(window, title);
}

void Tether::IWindow::SetBoundsEnabled(bool enabled)
{
	boundsEnabled = enabled;
}

void Tether::IWindow::SetBounds(int64_t minWidth, int64_t minHeight, 
	int64_t maxWidth, int64_t maxHeight)
{
	this->minWidth  = minWidth;
	this->minHeight = minHeight;
	this->maxWidth  = maxWidth;
	this->maxHeight = maxHeight;
}

void Tether::IWindow::SetDecorated(bool decorated)
{
	if (this->decorated == decorated)
		return;
	this->decorated = decorated;

	ReconstructStyle();
}

void Tether::IWindow::SetClosable(bool closable)
{
	if (this->closable == closable)
		return;
	this->closable = closable;

	ReconstructStyle();
}

void Tether::IWindow::SetResizable(bool isResizable)
{
	if (isResizable == this->resizable)
		return;
	this->resizable = isResizable;

	ReconstructStyle();
}

void Tether::IWindow::SetFullscreen(bool fullscreen, int monitor)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetFullscreen");

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
		SetWindowPos(window, HWND_TOP, x, y, width, height, SWP_SHOWWINDOW);
		ShowWindow(window, SW_SHOW);
	}
	
	this->fullscreen = fullscreen;
}

int64_t Tether::IWindow::GetX()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetX", 0);

	long unsigned int child;

	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);
	ClientToScreen(window, (LPPOINT)&windowRect.left);
	ClientToScreen(window, (LPPOINT)&windowRect.right);
	
	return windowRect.left;
}

int64_t Tether::IWindow::GetY()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetY", 0);

	long unsigned int child;

	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);
	ClientToScreen(window, (LPPOINT)&windowRect.left);
	ClientToScreen(window, (LPPOINT)&windowRect.right);
	
	return windowRect.top;
}

uint64_t Tether::IWindow::GetWidth()
{
	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);
	ClientToScreen(window, (LPPOINT)&windowRect.left);
	ClientToScreen(window, (LPPOINT)&windowRect.right);

	return windowRect.right - windowRect.left;
}

uint64_t Tether::IWindow::GetHeight()
{
	RECT windowRect;
	GetClientRect(window, (LPRECT)&windowRect);
	ClientToScreen(window, (LPPOINT)&windowRect.left);
	ClientToScreen(window, (LPPOINT)&windowRect.right);

	return windowRect.bottom - windowRect.top;
}

void Tether::IWindow::PollEvents()
{
	if (!initialized)
	{
		DispatchNoInit("IWindow::PollEvents");
		return;
	}
	
	MSG msg{};
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
		if (!IsWindow(window))
		{
			closeRequested = true;

			SpawnEvent(Events::EventType::WINDOW_CLOSING,
			[this](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
			});
		}
}

HINSTANCE Tether::IWindow::GetHIstance()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetHIstance", nullptr);
	return hinst;
}

HWND Tether::IWindow::GetHandle()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetHandle", 0);
	return window;
}

void Tether::IWindow::OnDispose()
{
	DestroyWindow(window);
	UnregisterClass(this->className.c_str(), hinst);
}

std::shared_ptr<wchar_t> Tether::IWindow::ToWide(const char* str)
{
	// Size to allocate
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	
	WCHAR* wstr = new WCHAR[size];
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)wstr, size);

	// Allocate shared pointer
	return std::shared_ptr<wchar_t>(wstr, std::default_delete<wchar_t[]>());
}

RECT Tether::IWindow::GetAdjustedRect()
{
	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);

	return wr;
}

void Tether::IWindow::ReconstructStyle()
{
	LONG style = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	SetWindowLong(window, GWL_STYLE, style);
}

LRESULT Tether::IWindow::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
{
	using namespace Tether::Events;

	// The window variable should never be used in this function because
	// the CreateWindow function sends events before it has finished execution;
	// therefore, it is possible for the window variable to be null. 
	// Use hWnd instead. Why does winapi do this? I have no idea.
	
	//bool eventReceived = false;
	//while (XPending(display))
	//{
	//	XNextEvent(display, &event);
	//	eventReceived = true;

	//	switch (event.type)
	//	{
	//	case MotionNotify:
	//	{
	//		if (!prevReceivedMouseMove)
	//		{
	//			mouseX = event.xmotion.x_root;
	//			mouseY = event.xmotion.y_root;
	//			relMouseX = event.xmotion.x;
	//			relMouseY = event.xmotion.y;
	//		}

	//		MouseMoveEvent linkEvent(
	//			event.xmotion.x_root,
	//			event.xmotion.y_root,
	//			event.xmotion.x,
	//			event.xmotion.y,
	//			relMouseX,
	//			relMouseY,
	//			mouseX,
	//			mouseY
	//		);

	//		SpawnEvent(Events::EventType::MOUSE_MOVE,
	//			[&](Events::EventHandler* pEventHandler)
	//			{
	//				pEventHandler->OnMouseMove(linkEvent);
	//			});

	//		mouseX = event.xmotion.x_root;
	//		mouseY = event.xmotion.y_root;
	//		relMouseX = event.xmotion.x;
	//		relMouseY = event.xmotion.y;

	//		prevReceivedMouseMove = true;
	//	}
	//	break;

	//	case ConfigureNotify:
	//	{
	//		XConfigureEvent xce = event.xconfigure;

	//		// Verify that the event was a resize event
	//		if (xce.width == width && xce.height == height)
	//			break;

	//		WindowResizeEvent linkEvent(
	//			xce.width,
	//			xce.height
	//		);

	//		SpawnEvent(Events::EventType::WINDOW_RESIZE,
	//			[&](Events::EventHandler* pEventHandler)
	//			{
	//				pEventHandler->OnWindowResize(linkEvent);
	//			});

	//		width = xce.width;
	//		height = xce.height;
	//	}
	//	break;

	//	case ClientMessage:
	//	{
	//		// Check for WM_PROTOCOLS
	//		if (event.xclient.message_type !=
	//			XInternAtom(display, "WM_PROTOCOLS", false))
	//			break;

	//		closeRequested = true;

	//		SpawnEvent(Events::EventType::WINDOW_CLOSING,
	//			[this](Events::EventHandler* pEventHandler)
	//			{
	//				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
	//			});
	//	}
	//	break;
	//	}
	//}

	switch (msg)
	{
		case WM_CLOSE:
		{
			closeRequested = true;

			SpawnEvent(Events::EventType::WINDOW_CLOSING,
			[this](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
			});

			return 0;
		}
		break;

		case WM_MOUSEMOVE:
		{
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);

			MouseMoveEvent event(
				this->x + x,
				this->y + y,
				x,
				y,
				relMouseX,
				relMouseY,
				mouseX,
				mouseY
			);

			SpawnEvent(Events::EventType::MOUSE_MOVE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnMouseMove(event);
			});

			mouseX = this->x + x;
			mouseY = this->y + y;
			relMouseX = x;
			relMouseY = y;

			prevReceivedMouseMove = true;
		}
		break;

		case WM_MOVE:
		{
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);

			WindowMoveEvent event(
				x,
				y
			);

			SpawnEvent(Events::EventType::WINDOW_MOVE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowMove(event);
			});

			this->x = x;
			this->y = y;
		}
		break;

		case WM_SIZING:
		{
			RECT bounds = *(RECT*)lParam;

			WindowResizeEvent event(
				bounds.right - bounds.left,
				bounds.bottom - bounds.top
			);

			SpawnEvent(Events::EventType::WINDOW_RESIZE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowResize(event);
			});

			width = event.GetNewWidth();
			height = event.GetNewHeight();
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

#endif //__linux__