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
	public:
		X11Window(int width, int height, std::wstring_view title, 
			bool visible);
		~X11Window();

		void Run() override;

		void SetVisible(bool visibility) override;
		bool IsVisible() override;
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
		void SetFullscreen(bool fullscreen, const FullscreenSettings& settings,
			const Devices::Monitor& monitor) override;
		void PollEvents() override;
		int GetX() override;
		int GetY() override;
		int GetWidth() override;
		int GetHeight() override;
		int GetMouseX() override;
		int GetMouseY() override;
		int GetRelativeMouseX() override;
		int GetRelativeMouseY() override;
		bool IsFocused() override;
	private:
		void ProcessEvent(XEvent& event);
		void ProcessMwmFunctions();

		int m_PrevX = 0, m_PrevY = 0;
		int m_PrevWidth = 0, m_PrevHeight = 0;

		bool m_Visible = false;
		bool m_Closable = true;
		bool m_Resizable = true;
		bool m_RawInputEnabled = false;
		bool m_Fullscreen = false;
		uint8_t m_StyleMask = ButtonStyleMask::MAXIMIZE_BUTTON
			                | ButtonStyleMask::MINIMIZE_BUTTON;

		bool m_BoundsEnabled = false;
		int m_MinWidth  = INT_MIN; 
		int m_MinHeight = INT_MIN;
		int m_MaxWidth  = INT_MAX;
		int m_MaxHeight = INT_MAX;

		bool m_PrevReceivedMouseMove = false;
		int m_MouseX = INT_MIN;
		int m_MouseY = INT_MIN;
		int m_RelMouseX = INT_MIN;
		int m_RelMouseY = INT_MIN;

		unsigned long m_Window = 0;

		uint32_t m_LastPressed = UINT32_MAX;

		using ConvertType = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<ConvertType, wchar_t> m_WideConverter;

		X11Application& m_App;
	};
}
