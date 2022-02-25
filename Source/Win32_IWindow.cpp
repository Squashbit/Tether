#ifdef _WIN32

#include <Tether/IWindow.hpp>
#include <Tether/Controls/Control.hpp>

#include <algorithm>
#include <cmath>

#include <string.h>

#define TETHER_WINSTYLE() GetWindowLong(window, GWL_STYLE)

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
	
	// Create window
	window = CreateWindowExA(
		0, // Extended style
		title,
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
	
	initialized = true;

	if (fullscreenMode)
	{
		// TODO: Monitor window hint
		SetFullscreen(true, 0);
	}

	if (shouldShow)
		ShowWindow(window, SW_SHOW);
	
	OnInit();

	return true;
}

void Tether::IWindow::SetVisible(bool visibility)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetVisible");

	if (visibility)
		ShowWindow(window, SW_SHOW);
	else
		ShowWindow(window, SW_HIDE);
	
	visible = visibility;
}

bool Tether::IWindow::IsVisible()
{
	return visible && initialized;
}

void Tether::IWindow::SetX(int64_t x)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetX");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetY(int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetY");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetPosition(int64_t x, int64_t y)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetPosition");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetWidth(uint64_t width)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetWidth");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetHeight(uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetHeight");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetSize(uint64_t width, uint64_t height)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetSize");

	RECT wr;
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);
	MoveWindow(window, wr.left, wr.top, wr.right, wr.bottom, false);
}

void Tether::IWindow::SetTitle(const char* title)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetTitle");

	SetWindowTextA(window, title);
}

void Tether::IWindow::SetBounds(int minWidth, int minHeight, int maxWidth, 
	int maxHeight)
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

	ProcessMwmFunctions();
}

void Tether::IWindow::SetResizable(bool isResizable)
{
	if (isResizable == resizable)
		return;
	resizable = isResizable;

	ProcessMwmFunctions();
}

void Tether::IWindow::SetFullscreen(bool fullscreen, int monitor)
{
	TETHER_ASSERT_INITIALIZED("IWindow::SetFullscreen");

	if (this->fullscreen == fullscreen)
		return;

	Atom wmState = XInternAtom(display, "_NET_WM_STATE", true);
	Atom fullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", 
			true);
	if (fullscreen)
	{
		XEvent fullscreenEvent{};
		fullscreenEvent.type = ClientMessage;
		fullscreenEvent.xclient.window = window;
		fullscreenEvent.xclient.message_type = wmState;
		fullscreenEvent.xclient.format = 32;
		fullscreenEvent.xclient.data.l[0] = 2; // Replace
		fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
		
		XSendEvent(display, DefaultRootWindow(display), false, 
			SubstructureRedirectMask | SubstructureNotifyMask, 
			&fullscreenEvent);
		
		XSync(display, false);
		
		Atom wmFullscreenMonitors = 
			XInternAtom(display, "_NET_WM_FULLSCREEN_MONITORS", true);
		XEvent event{};
		event.type = ClientMessage;
		event.xclient.window = window;
		event.xclient.message_type = wmFullscreenMonitors;
		event.xclient.format = 32;
		event.xclient.data.l[0] = monitor;
		event.xclient.data.l[1] = monitor;
		event.xclient.data.l[2] = monitor;
		event.xclient.data.l[3] = monitor;

		XSendEvent(display, DefaultRootWindow(display), false, 
			SubstructureRedirectMask | SubstructureNotifyMask, &event);
	}
	else
	{
		XEvent fullscreenEvent{};
		fullscreenEvent.type = ClientMessage;
		fullscreenEvent.xclient.window = window;
		fullscreenEvent.xclient.message_type = wmState;
		fullscreenEvent.xclient.format = 32;
		fullscreenEvent.xclient.data.l[0] = 0; // Remove
		fullscreenEvent.xclient.data.l[1] = fullscreenAtom;
		
		XSendEvent(display, DefaultRootWindow(display), false, 
			SubstructureRedirectMask | SubstructureNotifyMask, 
			&fullscreenEvent);
	}

	XSync(display, false);
	
	this->fullscreen = fullscreen;
}

int64_t Tether::IWindow::GetX()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetX", 0);

	long unsigned int child;

	int x, y;
	XTranslateCoordinates(display, window, 
		DefaultRootWindow(display), 0, 0, &x, &y, &child);
	
	XWindowAttributes attribs;
	XGetWindowAttributes(display, window, &attribs);
	
	return x - attribs.x;
}

int64_t Tether::IWindow::GetY()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetY", 0);

	long unsigned int child;

	int x, y;
	XTranslateCoordinates(display, window, 
		DefaultRootWindow(display), 0, 0, &x, &y, &child);
	
	XWindowAttributes attribs;
	XGetWindowAttributes(display, window, &attribs);
	
	return y - attribs.y;
}

uint64_t Tether::IWindow::GetWidth()
{
	XWindowAttributes attribs;
	XGetWindowAttributes(display, window, &attribs);

	return attribs.width;
}

uint64_t Tether::IWindow::GetHeight()
{
	XWindowAttributes attribs;
	XGetWindowAttributes(display, window, &attribs);

	return attribs.height;
}

void Tether::IWindow::PollEvents()
{
	using namespace Events;

	if (!initialized)
	{
		DispatchNoInit("IWindow::PollEvents");
		return;
	}
	
	if (!visible)
		return;

	bool eventReceived = false;
	while (XPending(display))
	{
		XNextEvent(display, &event);
		eventReceived = true;

		switch (event.type)
		{
			case MotionNotify:
			{
				if (!prevReceivedMouseMove)
				{
					mouseX = event.xmotion.x_root;
					mouseY = event.xmotion.y_root;
					relMouseX = event.xmotion.x;
					relMouseY = event.xmotion.y;
				}

				MouseMoveEvent linkEvent(
					event.xmotion.x_root,
					event.xmotion.y_root,
					event.xmotion.x,
					event.xmotion.y,
					relMouseX,
					relMouseY,
					mouseX,
					mouseY
				);
				
				SpawnEvent(Events::EventType::MOUSE_MOVE, 
				[&](Events::EventHandler* pEventHandler)
				{
					pEventHandler->OnMouseMove(linkEvent);
				});

				mouseX = event.xmotion.x_root;
				mouseY = event.xmotion.y_root;
				relMouseX = event.xmotion.x;
				relMouseY = event.xmotion.y;

				prevReceivedMouseMove = true;
			}
			break;

			case ConfigureNotify:
			{
				XConfigureEvent xce = event.xconfigure;

				// Verify that the event was a resize event
				if (xce.width == width && xce.height == height)
					break;
				
				WindowResizeEvent linkEvent(
					xce.width,
					xce.height
				);

				SpawnEvent(Events::EventType::WINDOW_RESIZE, 
				[&](Events::EventHandler* pEventHandler)
				{
					pEventHandler->OnWindowResize(linkEvent);
				});

				width = xce.width;
				height = xce.height;
			}
			break;

			case ClientMessage:
			{
				// Check for WM_PROTOCOLS
				if (event.xclient.message_type != 
					XInternAtom(display, "WM_PROTOCOLS", false))
					break;
				
				closeRequested = true;

				SpawnEvent(Events::EventType::WINDOW_CLOSING, 
				[this](Events::EventHandler* pEventHandler)
				{
					pEventHandler->OnWindowClosing(Events::WindowClosingEvent());
				});
			}
			break;
		}
	}
}

Display* Tether::IWindow::GetDisplay()
{
	return display;
}

int Tether::IWindow::GetScreen()
{
	return screen;
}

uint64_t Tether::IWindow::GetHandle()
{
	TETHER_ASSERT_INITIALIZED_RET("IWindow::GetHandle", 0);

	return window;
}

void Tether::IWindow::OnDispose()
{
	XLockDisplay(display);
		XUnmapWindow(display, window);
		XDestroyWindow(display, window);
		XFlush(display);
	XUnlockDisplay(display);

	XCloseDisplay(display);

	hints.clear();
}

void Tether::IWindow::ProcessMwmFunctions()
{
	Atom motifWmHints = XInternAtom(display, "_MOTIF_WM_HINTS", true);

	MwmHints hints{};
	hints.flags = MWM_HINTS_FUNCTIONS;
	hints.functions = MWM_FUNC_MOVE | MWM_FUNC_MAXIMIZE;

	if (closable)
		hints.functions |= MWM_FUNC_CLOSE;
	if (resizable)
		hints.functions |= MWM_FUNC_RESIZE;
	
	XChangeProperty(
		display, window,
		motifWmHints,
		motifWmHints, 
		32,
		PropModeReplace,
		(unsigned char*)&hints,
		sizeof(hints) / sizeof(long)
	);
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
	wr.left = x;
	wr.top = y;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, TETHER_WINSTYLE(), false);

	return wr;
}

void Tether::IWindow::ReconstructStyle()
{
	LONG style = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	SetWindowLong(window, GWL_STYLE, style);
}

#endif //__linux__