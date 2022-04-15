#ifdef _WIN32

#include <Tether/IWindow.hpp>
#include <Tether/Controls/Control.hpp>
#include <Tether/Common/StringUtils.hpp>
#include <Tether/Native.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

#include <Windows.h>

using namespace Tether::Storage;

int64_t Tether::WindowProcCaller::HandleMessage(void* hWnd, Tether::IWindow* pWnd,
	uint32_t msg, uint64_t wParam, uint64_t lParam)
{
	HWND* window = &((VarStorage*)pWnd->varStorage)->window;

	if (*window == NULL)
		*window = *(HWND*)hWnd;

	return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
}

static RECT Tether_GetAdjustedRect(Tether::IWindow* pWnd, 
	int64_t x, int64_t y, uint64_t width, uint64_t height)
{
	RECT wr;
	wr.left = (LONG)x;
	wr.top = (LONG)y;
	wr.right = wr.left + (LONG)width;
	wr.bottom = wr.top + (LONG)height;

	AdjustWindowRect(&wr, pWnd->CalculateStyle(), false);

	return wr;
}

static Tether::WindowProcCaller caller;

static LRESULT CALLBACK Tether_WndProcRedir(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
{
	Tether::IWindow* pWindow = reinterpret_cast<Tether::IWindow*>(
		GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return caller.HandleMessage(&hWnd, pWindow, msg, wParam, lParam);
}

static LRESULT CALLBACK Tether_WindowProc(HWND hWnd, UINT msg, WPARAM wParam, 
	LPARAM lParam)
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
	storage->hinst = GetModuleHandle(NULL);
	if (!storage->hinst)
		return false;
	
	this->setWidth = width;
	this->setHeight = height;
	this->closeRequested = false;
	
	bool shouldShow = true;
	for (uint64_t i = 0; i < hints.size(); i++)
	{
		int64_t value = hints[i].value;
		switch (hints[i].type)
		{
			case HintType::X: this->setX = value; break;
			case HintType::Y: this->setY = value; break;
			case HintType::VISIBLE: shouldShow = (bool)value; break;
		}
	}

	RECT wr = Tether_GetAdjustedRect(this, setX, setY, setWidth, setHeight);

	// Generate class name
	this->className = StringUtils::RandomString(20);

	// Create class
	storage->wndClass.cbSize = sizeof(WNDCLASSEX);
	storage->wndClass.style = CS_HREDRAW | CS_VREDRAW;
	storage->wndClass.cbClsExtra = 0;
	storage->wndClass.cbWndExtra = 0;
	storage->wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	storage->wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	storage->wndClass.hIcon = LoadIcon(storage->hinst, IDI_APPLICATION);
	storage->wndClass.hIconSm = LoadIcon(storage->hinst, IDI_APPLICATION);
	storage->wndClass.lpszMenuName = nullptr;
	storage->wndClass.lpszClassName = className.c_str();
	storage->wndClass.hInstance = storage->hinst;
	storage->wndClass.lpfnWndProc = Tether_WindowProc;

	if (!RegisterClassEx(&storage->wndClass))
		return false;
	
	// Create window
	storage->window = CreateWindowEx(
		0, // Extended style
		className.c_str(),
		title,
		CalculateStyle(),
		wr.left,
		wr.top,
		wr.right - wr.left, // Width
		wr.bottom - wr.top, // Height
		nullptr, //Handle to the parent of this window
		nullptr, //Handle to the menu of child window identifier
		storage->hinst,
		this
	);
	if (!storage->window)
	{
		closeRequested = true;
		return false;
	}
	
	initialized = true;

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
		ShowWindow(storage->window, SW_SHOW);
		UpdateWindow(storage->window);
	}
	else
		ShowWindow(storage->window, SW_HIDE);
	
	visible = visibility;
}

bool Tether::IWindow::IsVisible()
{
	return visible && initialized;
}

void Tether::IWindow::SetRawInputEnabled(bool enabled)
{
	// TODO
}

void Tether::IWindow::SetCursorMode(CursorMode mode)
{
	switch (mode)
	{
		case CursorMode::NORMAL:
		{
			ShowCursor(true);
		}
		break;

		case CursorMode::HIDDEN:
		{
			ShowCursor(false);
		}
		break;
	}
}

void Tether::IWindow::SetMousePos(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	ClientToScreen(storage->window, &pt);
	SetCursorPos(pt.x, pt.y);
}

void Tether::IWindow::SetMouseRootPos(int x, int y)
{
	SetCursorPos(x, y);
}

void Tether::IWindow::SetX(int64_t x)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetX");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		static_cast<int>(x), 
		wr.top, 
		wr.right - wr.left, 
		wr.bottom - wr.top, 
		false
	);

	this->setX = x;
}

void Tether::IWindow::SetY(int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetY");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		wr.left,
		static_cast<int>(y),
		wr.right - wr.left,
		wr.bottom - wr.top,
		false
	);

	this->setY = y;
}

void Tether::IWindow::SetPosition(int64_t x, int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetPosition");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		static_cast<int>(x),
		static_cast<int>(y),
		wr.right - wr.left,
		wr.bottom - wr.top,
		false
	);

	this->setX = x;
	this->setY = y;
}

void Tether::IWindow::SetWidth(uint64_t width)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetWidth");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		wr.left,
		wr.top,
		static_cast<int>(width),
		wr.bottom - wr.top,
		false
	);

	this->setWidth = width;
}

void Tether::IWindow::SetHeight(uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetHeight");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		wr.left,
		wr.top,
		wr.right - wr.left,
		static_cast<int>(height),
		false
	);

	this->setHeight = height;
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetSize");

	RECT wr;
	GetWindowRect(storage->window, &wr);

	MoveWindow(
		storage->window,
		wr.left,
		wr.top,
		static_cast<int>(width),
		static_cast<int>(height),
		false
	);

	this->setWidth = width;
	this->setHeight = height;
}

void Tether::IWindow::SetTitle(const char* title)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetTitle");

	SetWindowTextA(storage->window, title);
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

void Tether::IWindow::SetMinimizeBox(bool minimizeBox)
{
	if (minimizeBox == this->minimizeBox)
		return;
	this->minimizeBox = minimizeBox;

	ReconstructStyle();
}

void Tether::IWindow::SetMaximizeBox(bool maximizeBox)
{
	if (maximizeBox == this->maximizeBox)
		return;
	this->maximizeBox = maximizeBox;

	ReconstructStyle();
}

void Tether::IWindow::SetMaximized(bool maximize)
{
	if (visible)
		if (maximize)
			ShowWindow(storage->window, SW_MAXIMIZE);
		else
			ShowWindow(storage->window, SW_SHOW);
}

void Tether::IWindow::SetFullscreen(
	bool fullscreen,
	FullscreenSettings* settings,
	Monitor* monitor
)
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

		SetWindowLongPtr(storage->window, GWL_STYLE, WS_POPUP);
		SetWindowPos(storage->window, HWND_TOP, 0, 0, dmScreenSettings.dmPelsWidth,
			dmScreenSettings.dmPelsHeight, SWP_SHOWWINDOW);
		ShowWindow(storage->window, SW_MAXIMIZE);
	}
	else
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);

		ChangeDisplaySettings(&dmScreenSettings, CDS_RESET);

		ReconstructStyle();
		SetWindowPos(storage->window, HWND_TOP, 
			static_cast<int>(setX), 
			static_cast<int>(setY), 
			static_cast<int>(setWidth), 
			static_cast<int>(setHeight),
			SWP_SHOWWINDOW
		);
		ShowWindow(storage->window, SW_SHOW);
	}
	
	this->fullscreen = fullscreen;
}

int64_t Tether::IWindow::GetX()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetX", 0);

	RECT windowRect;
	GetWindowRect(storage->window, (LPRECT)&windowRect);
	
	return windowRect.left;
}

int64_t Tether::IWindow::GetY()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetY", 0);

	RECT windowRect;
	GetWindowRect(storage->window, (LPRECT)&windowRect);
	
	return windowRect.top;
}

uint64_t Tether::IWindow::GetWidth()
{
	RECT windowRect;
	GetClientRect(storage->window, (LPRECT)&windowRect);
	
	return windowRect.right - windowRect.left;
}

uint64_t Tether::IWindow::GetHeight()
{
	RECT windowRect;
	GetClientRect(storage->window, (LPRECT)&windowRect);

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
	while (PeekMessage(&msg, storage->window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
		if (!IsWindow(storage->window))
		{
			if (!closable)
				return;
			
			closeRequested = true;

			SpawnEvent(Events::EventType::WINDOW_CLOSING,
			[this](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
			});
		}
}

void Tether::IWindow::OnDispose()
{
	DestroyWindow(storage->window);
	UnregisterClass(this->className.c_str(), storage->hinst);
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

unsigned long Tether::IWindow::CalculateStyle()
{
	LONG style = WS_OVERLAPPEDWINDOW | WS_SYSMENU;

	// TODO: Resize stuff

	if (maximizeBox) style |= WS_MAXIMIZEBOX;
	if (minimizeBox) style |= WS_MINIMIZEBOX;
	if (!decorated)  style |= WS_POPUP;
	
	return style;
}

void Tether::IWindow::ReconstructStyle()
{
	SetWindowLongPtr(storage->window, GWL_STYLE, CalculateStyle());
	UpdateWindow(storage->window);
}

int64_t Tether::IWindow::HandleMessage(void* pHWnd, uint32_t msg, uint64_t wParam,
	uint64_t lParam)
{
	using namespace Tether::Events;

	// The window variable should never be used in this function because
	// the CreateWindow function sends events before it has finished execution;
	// therefore, it is possible for the window variable to be null. 
	// Use hWnd instead. Why does winapi do this? I have no idea.

	HWND hWnd = *(HWND*)pHWnd;

	switch (msg)
	{
		case WM_CLOSE:
		{
			if (!closable)
				break;

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

			POINT mouse;
			mouse.x = x;
			mouse.y = y;

			ClientToScreen(storage->window, &mouse);

			if (x == relMouseX && y == relMouseY)
				break;

			if (!prevReceivedMouseMove)
			{
				mouseX = mouse.x;
				mouseY = mouse.y;
				relMouseX = x;
				relMouseY = y;
			}

			MouseMoveEvent event(
				mouse.x,
				mouse.y,
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

			SpawnEvent(Events::EventType::WINDOW_MOVE,
			[&](Events::EventHandler* pEventHandler)
			{
				pEventHandler->OnWindowMove(event);
			});
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

			SpawnEvent(Events::EventType::WINDOW_RESIZE,
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