#include <Tether/Platform/Win32Window.hpp>

#include <Tether/Common/StringTools.hpp>


#include <algorithm>
#include <random>
#include <cmath>
#include <cstring>

#include <WindowsX.h>

#undef ERROR

namespace Tether::Platform
{
	class HandleMessageCaller
	{
	public:
		HandleMessageCaller(Win32Window& window)
			:
			m_Window(window)
		{}

		LRESULT HandleMessage(HWND hwnd, DWORD msg, WPARAM wparam, LPARAM lparam)
		{
			return m_Window.HandleMessage(hwnd, msg, wparam, lparam);
		}
	private:
		Win32Window& m_Window;
	};

	static LRESULT CALLBACK WndProcRedir(HWND hWnd, UINT msg, WPARAM wParam,
		LPARAM lParam)
	{
		HandleMessageCaller caller(*(Win32Window*)GetWindowLongPtr(hWnd, 
			GWLP_USERDATA));
		return caller.HandleMessage(hWnd, msg, wParam, lParam);
	}

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam,
		LPARAM lParam)
	{
		// This hwnd procedure is only to initialize the hwnd user data before
		// using the actual redirection hwnd procedure.

		if (msg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			Win32Window* window = reinterpret_cast<Win32Window*>(
				pCreate->lpCreateParams);

			// Sanity check
			if (window == nullptr)
				return 0;

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			// Switch the hwnd procedure
			SetWindowLongPtr(hWnd, GWLP_WNDPROC,
				reinterpret_cast<LONG_PTR>(WndProcRedir));
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Win32Window::Win32Window(int width, int height, std::wstring_view title, 
		bool visible)
	{
		// HInstance doesn't need to be from the WinMain entrypoint.
		m_Hinst = GetModuleHandle(NULL);

		m_Width = width;
		m_Height = height;

		RECT wr = GetAdjustedRect(0, 0, width, height);

		GenerateClassName();

		// Create class
		m_WndClass.cbSize = sizeof(WNDCLASSEX);
		m_WndClass.style = CS_HREDRAW | CS_VREDRAW;
		m_WndClass.cbClsExtra = 0;
		m_WndClass.cbWndExtra = 0;
		m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		m_WndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		m_WndClass.hIcon = LoadIcon(m_Hinst, IDI_APPLICATION);
		m_WndClass.hIconSm = LoadIcon(m_Hinst, IDI_APPLICATION);
		m_WndClass.lpszMenuName = nullptr;
		m_WndClass.lpszClassName = m_ClassName.c_str();
		m_WndClass.hInstance = m_Hinst;
		m_WndClass.lpfnWndProc = WindowProc;

		RegisterClassEx(&m_WndClass);

		// Create hwnd
		m_Hwnd = CreateWindowEx(
			0, // Extended style
			m_ClassName.c_str(),
			title.data(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left, // Width
			wr.bottom - wr.top, // Height
			nullptr, //Handle to the parent of this hwnd
			nullptr, //Handle to the menu of child hwnd identifier
			m_Hinst,
			this
		);

		if (visible)
			SetVisible(true);
	}

	Win32Window::~Win32Window()
	{
		DestroyWindow(m_Hwnd);
		UnregisterClass(m_ClassName.c_str(), m_Hinst);
	}

	void Win32Window::Run()
	{
		using namespace Events;

		MSG msg;
		while (GetMessage(&msg, m_Hwnd, 0, 0) > 0 && !IsCloseRequested())
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (m_CursorMode == CursorMode::DISABLED
				&& GetForegroundWindow() == m_Hwnd)
			{
				SetCursorPos(
					static_cast<int>(GetWidth() / 2),
					static_cast<int>(GetHeight() / 2)
				);
			}
		}
	}

	void Win32Window::SetVisible(bool visibility)
	{
		if (visibility)
		{
			ShowWindow(m_Hwnd, SW_SHOW);
			ReconstructStyle();
		}
		else
			ShowWindow(m_Hwnd, SW_HIDE);

		m_Visible = visibility;
	}

	bool Win32Window::IsVisible()
	{
		return m_Visible;
	}

	void Win32Window::SetRawInputEnabled(bool enabled)
	{
		if (enabled && !m_RawInputInitialized)
		{
			RAWINPUTDEVICE rawInputDevice;
			rawInputDevice.usUsagePage = 0x01; // Mouse
			rawInputDevice.usUsage = 0x02;
			rawInputDevice.dwFlags = 0;
			rawInputDevice.hwndTarget = m_Hwnd;

			if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
				return;

			m_RawInputInitialized = true;
		}

		m_RawInputEnabled = enabled;
	}

	void Win32Window::SetCursorMode(CursorMode mode)
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

		m_CursorMode = mode;
	}

	static void SetCurPos(int x, int y)
	{
		SetCursorPos(x, y);
	}

	void Win32Window::SetCursorPos(int x, int y)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		ClientToScreen(m_Hwnd, &pt);
		SetCurPos(pt.x, pt.y);
	}

	void Win32Window::SetCursorRootPos(int x, int y)
	{
		SetCurPos(x, y);
	}

	void Win32Window::SetX(int x)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			static_cast<int>(x),
			wr.top,
			wr.right - wr.left,
			wr.bottom - wr.top,
			false
		);

		m_X = x;
	}

	void Win32Window::SetY(int y)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			wr.left,
			static_cast<int>(y),
			wr.right - wr.left,
			wr.bottom - wr.top,
			false
		);

		m_Y = y;
	}

	void Win32Window::SetPosition(int x, int y)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			static_cast<int>(x),
			static_cast<int>(y),
			wr.right - wr.left,
			wr.bottom - wr.top,
			false
		);

		m_X = x;
		m_Y = y;
	}

	void Win32Window::SetWidth(int width)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			wr.left,
			wr.top,
			width,
			wr.bottom - wr.top,
			false
		);

		m_Width = width;
	}

	void Win32Window::SetHeight(int height)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			wr.left,
			wr.top,
			wr.right - wr.left,
			height,
			false
		);

		m_Height = height;
	}

	void Win32Window::SetSize(int width, int height)
	{
		RECT wr;
		GetWindowRect(m_Hwnd, &wr);

		MoveWindow(
			m_Hwnd,
			wr.left,
			wr.top,
			width,
			height,
			false
		);

		m_Width = width;
		m_Height = height;
	}

	void Win32Window::SetTitle(std::wstring_view title)
	{
		SetWindowText(m_Hwnd, title.data());
	}

	void Win32Window::SetBoundsEnabled(bool enabled)
	{
		m_BoundsEnabled = enabled;
	}

	void Win32Window::SetBounds(int minWidth, int minHeight,
		int maxWidth, int maxHeight)
	{
		minWidth = minWidth;
		minHeight = minHeight;
		maxWidth = maxWidth;
		maxHeight = maxHeight;
	}

	void Win32Window::SetDecorated(bool decorated)
	{
		m_Decorated = decorated;

		ReconstructStyle();
	}

	void Win32Window::SetResizable(bool resizable)
	{
		m_Resizable = resizable;

		ReconstructStyle();
	}

	void Win32Window::SetClosable(bool closable)
	{
		m_Closable = closable;
	}

	void Win32Window::SetButtonStyleBitmask(uint8_t style)
	{
		m_StyleMask = style;

		ReconstructStyle();
	}

	void Win32Window::SetMaximized(bool maximize)
	{
		if (m_Visible)
			if (maximize)
				ShowWindow(m_Hwnd, SW_MAXIMIZE);
			else
				ShowWindow(m_Hwnd, SW_SHOW);
	}

	void Win32Window::SetPreferredResizeInc(int x, int y)
	{}

	void Win32Window::SetFullscreen(bool fullscreen, const Devices::Monitor& monitor)
	{
		if (m_Fullscreen == fullscreen)
			return;

		if (fullscreen)
		{
			SetWindowLong(m_Hwnd, GWL_STYLE, WS_POPUP);
			SetWindowPos(m_Hwnd, HWND_TOP, monitor.GetX(), monitor.GetY(), 
				monitor.GetWidth(), monitor.GetHeight(), SWP_SHOWWINDOW);
			ShowWindow(m_Hwnd, SW_MAXIMIZE);
		}
		else
		{
			DEVMODEW dmScreenSettings{};
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);

			ChangeDisplaySettings(&dmScreenSettings, CDS_RESET);

			ReconstructStyle();
			SetWindowPos(m_Hwnd, HWND_TOP,
				static_cast<int>(m_X),
				static_cast<int>(m_Y),
				static_cast<int>(m_Width),
				static_cast<int>(m_Height),
				SWP_SHOWWINDOW
			);
			ShowWindow(m_Hwnd, SW_SHOW);
		}

		m_Fullscreen = fullscreen;
	}

	int Win32Window::GetX()
	{
		return m_X;
	}

	int Win32Window::GetY()
	{
		return m_Y;
	}

	int Win32Window::GetWidth()
	{
		return m_Width;
	}

	int Win32Window::GetHeight()
	{
		return m_Height;
	}

	int Win32Window::GetMouseX()
	{
		return m_MouseX;
	}

	int Win32Window::GetMouseY()
	{
		return m_MouseY;
	}

	int Win32Window::GetRelativeMouseX()
	{
		return m_RelMouseX;
	}

	int Win32Window::GetRelativeMouseY()
	{
		return m_RelMouseY;
	}

	bool Win32Window::IsFocused()
	{
		return GetForegroundWindow() == m_Hwnd;
	}

	HWND Win32Window::GetHWND()
	{
		return m_Hwnd;
	}

	HINSTANCE Win32Window::GetHINSTANCE()
	{
		return m_Hinst;
	}

	void Win32Window::PollEvents()
	{
		if (!m_Visible)
			return;

		using namespace Events;

		MSG msg{};
		while (PeekMessage(&msg, m_Hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_NULL)
			if (!IsWindow(m_Hwnd))
			{
				if (!m_Closable)
					return;

				SetCloseRequested(true);
				SpawnEvent(Events::EventType::WINDOW_CLOSING,
					[this](Events::EventHandler& eventHandler)
				{
					eventHandler.OnWindowClosing(Events::WindowClosingEvent());
				});
			}

		if (m_CursorMode == CursorMode::DISABLED 
			&& GetForegroundWindow() == m_Hwnd)
		{
			SetCursorPos(
				static_cast<int>(GetWidth() / 2),
				static_cast<int>(GetHeight() / 2)
			);
		}

		SpawnEvent(EventType::WINDOW_REPAINT,
			[&](EventHandler& eventHandler)
		{
			eventHandler.OnWindowRepaint(WindowRepaintEvent());
		});
	}

	LONG Win32Window::CalculateStyle()
	{
		LONG style = GetWindowLong(m_Hwnd, GWL_STYLE);
		
		if (!m_Decorated)
			style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
				| WS_SYSMENU);
		else
		{
			style |= WS_CAPTION | WS_SYSMENU;

			if (m_Resizable)
				style |= WS_THICKFRAME;
			else
				style &= ~WS_THICKFRAME;

			if (m_StyleMask & ButtonStyleMask::MINIMIZE_BUTTON)
				style |= WS_MINIMIZEBOX;
			else
				style &= ~WS_MINIMIZEBOX;

			if (m_StyleMask & ButtonStyleMask::MAXIMIZE_BUTTON)
				style |= WS_MAXIMIZEBOX;
			else
				style &= ~WS_MAXIMIZEBOX;
		}
		
		return style;
	}

	LONG Win32Window::CalculateExtendedStyle()
	{
		LONG exStyle = GetWindowLong(m_Hwnd, GWL_EXSTYLE);

		// Keep this. It might prove useful later.
		/*if (decorated)
			exStyle |= WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;*/
		
		return exStyle;
	}

	void Win32Window::ReconstructStyle()
	{
		SetWindowLong(m_Hwnd, GWL_STYLE, CalculateStyle());
		SetWindowLong(m_Hwnd, GWL_EXSTYLE, CalculateExtendedStyle());
		SetWindowPos(m_Hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE 
			| SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	RECT Win32Window::GetAdjustedRect(int x, int y, int width, int height)
	{
		RECT wr;
		wr.left = (LONG)x;
		wr.top = (LONG)y;
		wr.right = wr.left + (LONG)width;
		wr.bottom = wr.top + (LONG)height;

		AdjustWindowRect(&wr, CalculateStyle(), false);

		return wr;
	}

	LRESULT Win32Window::HandleMessage(HWND hWnd, DWORD msg, WPARAM wParam, 
		LPARAM lParam)
	{
		using namespace Events;
		using namespace Input;

		// The hwnd variable should never be used in this function because
		// the CreateWindow function sends events before it has finished execution;
		// therefore, it is possible for the hwnd variable to be null. 
		// Use hWnd instead. Why does WinAPI do this? I have no idea.

		using ClickType = MouseClickInfo::ClickType;

		uint32_t wParam32 = (uint32_t)wParam;

		switch (msg)
		{
			case WM_CLOSE:
			{
				if (!m_Closable)
					break;

				SetCloseRequested(true);
				SpawnEvent(Events::EventType::WINDOW_CLOSING,
					[this](Events::EventHandler& eventHandler)
				{
					eventHandler.OnWindowClosing(Events::WindowClosingEvent());
				});
			}
			break;

			case WM_KEYDOWN:
			{
				if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
					return 0;

				UINT scancode = HIWORD(lParam) & (KF_EXTENDED | 0xFF);
				if (!scancode)
					scancode = MapVirtualKey(wParam32, MAPVK_VK_TO_VSC);

				// Windows does this thing where you can't distinguish between either 
				// shift keys, so instead, just press both and hope for the best.
				if (wParam == VK_SHIFT)
				{
					SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, true);
					SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, true);

					return 0;
				}

				Application& application = Application::Get();
				const int16_t* keycodes = application.GetKeycodes();

				SpawnKeyInput(scancode, keycodes[wParam32], true);
			}
			break;

			case WM_KEYUP:
			{
				UINT scancode = MapVirtualKey(wParam32, MAPVK_VK_TO_VSC);

				if (wParam == VK_SHIFT)
				{
					SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, false);
					SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, false);

					return 0;
				}

				Application& application = Application::Get();
				const int16_t* keycodes = application.GetKeycodes();

				SpawnKeyInput(scancode, keycodes[wParam32], false);
				return 0;
			}
			break;

			case WM_CHAR:
			{
				SpawnEvent(EventType::WINDOW_REPAINT,
					[&](EventHandler& eventHandler)
				{
					eventHandler.OnWindowRepaint(WindowRepaintEvent());
				});

				Input::KeyCharInfo event(
					(char)wParam,
					(HIWORD(lParam) & KF_REPEAT) == KF_REPEAT
					);

				SpawnInput(Input::InputType::KEY_CHAR,
					[&](Input::InputListener& inputListener)
				{
					inputListener.OnKeyChar(event);
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

				ClientToScreen(m_Hwnd, &mouse);

				if (x == m_RelMouseX && y == m_RelMouseY)
					break;

				if (!m_PrevReceivedMouseMove)
				{
					m_MouseX = mouse.x;
					m_MouseY = mouse.y;
					m_RelMouseX = x;
					m_RelMouseY = y;
				}

				Input::MouseMoveInfo event(
					mouse.x,
					mouse.y,
					x,
					y,
					m_RelMouseX,
					m_RelMouseY,
					m_MouseX,
					m_MouseY
					);

				SpawnInput(Input::InputType::MOUSE_MOVE,
					[&](Input::InputListener& inputListener)
				{
					inputListener.OnMouseMove(event);
				});

				m_MouseX = mouse.x;
				m_MouseY = mouse.y;
				m_RelMouseX = x;
				m_RelMouseY = y;

				m_PrevReceivedMouseMove = true;
			}
			break;

			case WM_MOVE:
			{
				m_X = LOWORD(lParam);
				m_Y = HIWORD(lParam);

				WindowMoveEvent event(m_X, m_Y);
				SpawnEvent(Events::EventType::WINDOW_MOVE,
					[&](Events::EventHandler& eventHandler)
				{
					eventHandler.OnWindowMove(event);
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

						SpawnInput(Input::InputType::RAW_MOUSE_MOVE,
							[&](Input::InputListener& inputListener)
						{
							inputListener.OnRawMouseMove(event);
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

				m_Width = LOWORD(lParam);
				m_Height = HIWORD(lParam);

				if (m_Width == 0 && m_Height == 0)
					break;

				WindowResizeEvent event(m_Width, m_Height);
				SpawnEvent(Events::EventType::WINDOW_RESIZE,
					[&](Events::EventHandler& eventHandler)
				{
					eventHandler.OnWindowResize(event);
				});
			}
			break;

			case WM_GETMINMAXINFO:
			{
				if (!m_BoundsEnabled)
					break;

				MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
				pInfo->ptMinTrackSize = { (LONG)m_MinWidth,
					(LONG)m_MinHeight };
				pInfo->ptMaxTrackSize = { (LONG)m_MaxWidth,
					(LONG)m_MaxHeight };
			}
			break;

			case WM_PAINT:
			{
				SpawnEvent(EventType::WINDOW_REPAINT,
				[&](EventHandler& eventHandler)
				{
					eventHandler.OnWindowRepaint(WindowRepaintEvent());
				});

				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

			case WM_LBUTTONDOWN:
			{
				SpawnMouseClick(lParam, ClickType::LEFT_BUTTON, true);
			}
			break;

			case WM_LBUTTONUP:
			{
				SpawnMouseClick(lParam, ClickType::LEFT_BUTTON, false);
			}
			break;

			case WM_MBUTTONDOWN:
			{
				SpawnMouseClick(lParam, ClickType::MIDDLE_BUTTON, true);
			}
			break;

			case WM_MBUTTONUP:
			{
				SpawnMouseClick(lParam, ClickType::MIDDLE_BUTTON, false);
			}
			break;

			case WM_RBUTTONDOWN:
			{
				SpawnMouseClick(lParam, ClickType::RIGHT_BUTTON, true);
			}
			break;

			case WM_RBUTTONUP:
			{
				SpawnMouseClick(lParam, ClickType::RIGHT_BUTTON, false);
			}
			break;

			case WM_XBUTTONDOWN:
			{
				SpawnXbuttonClick(lParam, wParam, true);
			}
			break;

			case WM_XBUTTONUP:
			{
				SpawnXbuttonClick(lParam, wParam, false);
			}
			break;

			default: return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}

	void Win32Window::SpawnMouseClick(
		LPARAM lParam,
		Input::MouseClickInfo::ClickType type,
		bool pressed
	)
	{
		using namespace Input;

		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		POINT mouse;
		mouse.x = x;
		mouse.y = y;

		ClientToScreen(m_Hwnd, &mouse);

		MouseClickInfo event(
			(int)mouse.x,
			(int)mouse.y,
			x,
			y,
			type,
			pressed
		);

		SpawnInput(InputType::MOUSE_CLICK,
			[&](InputListener& inputListener)
		{
			inputListener.OnMouseClick(event);
		});
	}

	bool Win32Window::SpawnXbuttonClick(
		LPARAM lParam,
		WPARAM wParam,
		bool pressed
	)
	{
		using ClickType = Input::MouseClickInfo::ClickType;

		ClickType type = ClickType::SIDE_BUTTON1;
		switch (GET_XBUTTON_WPARAM(wParam))
		{
			case XBUTTON1: type = ClickType::SIDE_BUTTON1;
			case XBUTTON2: type = ClickType::SIDE_BUTTON2;

			default: return false;
		}

		SpawnMouseClick(lParam, type, pressed);

		return true;
	}

	void Win32Window::GenerateClassName()
	{
		static constexpr const size_t CLASS_NAME_LENGTH = 32;

		const std::wstring nums = L"0123456789";
		const std::wstring caps = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		const std::wstring lowers = L"abcdefghijklmnopqrstuvwxyz";
		const std::wstring allChars = nums + caps + lowers;

		std::random_device device;
		std::mt19937 generator(device());
		std::uniform_int_distribution<size_t> dist(0, allChars.size() - 1);

		for (size_t i = 0; i < CLASS_NAME_LENGTH; i++)
			m_ClassName += allChars[dist(generator)];
	}
}
