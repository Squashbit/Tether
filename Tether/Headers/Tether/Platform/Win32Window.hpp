#pragma once

#include <Tether/Window.hpp>

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h> // oh no

namespace Tether::Platform
{
	class TETHER_EXPORT Win32Window : public Window
	{
		friend class HandleMessageCaller;
	public:
		Win32Window(int width, int height, std::wstring_view title,
			bool visible = false);
		~Win32Window();

		void SetVisible(bool visibility) override;
		void SetRawInputEnabled(bool enabled) override;
		void SetCursorMode(CursorMode mode) override;
		void SetCursorPos(int x, int y) override;
		void SetCursorRootPos(int x, int y) override;
		void SetX(int x) override;
		void SetY(int y) override;
		void SetPosition(int x, int y) override;
		void SetWidth(int width) override;
		void SetHeight(int height) override;
		void SetSize(int width, int height) override;
		void SetTitle(std::wstring_view title) override;
		void SetBoundsEnabled(bool enabled) override;
		void SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight) override;
		void SetDecorated(bool enabled) override;
		void SetResizable(bool resizable) override;
		void SetClosable(bool closable) override;
		void SetButtonStyleBitmask(uint8_t mask) override;
		void SetMaximized(bool maximized) override;
		void SetPreferredResizeInc(int x, int y) override;
		void EnableFullscreen(const Devices::Monitor& monitor) override;
		void DisableFullscreen() override;
		bool IsFocused() override;
		bool IsVisible() override;

		HWND GetHWND();
		HINSTANCE GetHINSTANCE();
	protected:
		LRESULT HandleMessage(HWND hwnd, DWORD msg, WPARAM wparam, LPARAM lparam);
	private:
		void SpawnMouseClick(
			LPARAM lParam,
			Input::MouseClickInfo::ClickType type,
			bool pressed
		);

		bool SpawnXbuttonClick(
			LPARAM lParam,
			WPARAM wParam,
			bool pressed
		);

		void GenerateClassName();

		RECT GetAdjustedRect(int x, int y, int width, int height);
		void ReconstructStyle();
		LONG CalculateStyle();
		LONG CalculateExtendedStyle();

		HWND m_Hwnd = 0;
		HINSTANCE m_Hinst = nullptr;
	
		std::wstring m_ClassName;

		bool m_Decorated = true;
		bool m_Visible = false;
		bool m_Resizable = true;
		bool m_Closable = true;
		bool m_Fullscreen = false;

		bool m_RawInputEnabled = false;
		bool m_RawInputInitialized = false;
		Window::CursorMode m_CursorMode = Window::CursorMode::NORMAL;

		bool m_BoundsEnabled = false;
		int m_MinWidth = INT_MIN; 
		int m_MinHeight = INT_MIN;
		int m_MaxWidth = INT_MAX;
		int m_MaxHeight = INT_MAX;

		bool m_PrevReceivedMouseMove = false;
		
		uint8_t m_StyleMask = ButtonStyleMask::MAXIMIZE_BUTTON
			              | ButtonStyleMask::MINIMIZE_BUTTON;

		WNDCLASSEXW m_WndClass;
	};
}
