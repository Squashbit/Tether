#pragma once

#include <Tether/Platform/X11Application.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

#include <Tether/Window.hpp>

#include <locale>
#include <codecvt>

namespace Tether::Platform
{
	class TETHER_EXPORT X11Window : public Window
	{
		friend X11Application;
	public:
		X11Window(int width, int height, std::wstring_view title, 
			bool visible);
		~X11Window();

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

		unsigned long GetWindowHandle() const;
	private:
		void ProcessEvent(XEvent& event);
		void DispatchMouseButton(XEvent& event, bool pressed);
		
		void ProcessMwmFunctions();

		bool m_Visible = false;
		bool m_Closable = true;
		bool m_Resizable = true;
		bool m_RawInputEnabled = false;
		uint8_t m_StyleMask = ButtonStyleMask::MAXIMIZE_BUTTON
			                | ButtonStyleMask::MINIMIZE_BUTTON;

		bool m_BoundsEnabled = false;
		int m_MinWidth  = INT_MIN; 
		int m_MinHeight = INT_MIN;
		int m_MaxWidth  = INT_MAX;
		int m_MaxHeight = INT_MAX;

		bool m_PrevReceivedMouseMove = false;

		unsigned long m_Window = 0;

		uint32_t m_LastPressed = UINT32_MAX;

		using ConvertType = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<ConvertType, wchar_t> m_WideConverter;

		X11Application& m_App;
	};
}
