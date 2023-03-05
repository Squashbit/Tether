#pragma once
#ifdef __linux__

#include <Tether/Window.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

namespace Tether::Platform
{
	class TETHER_EXPORT X11Window : public Window
	{
	public:
		X11Window(int width, int height, std::wstring_view title, 
			bool visible);
		~X11Window();

		bool Run() override;

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
		void ProcessMwmFunctions();
		
		uint64_t prevX = 0, prevY = 0;
		uint64_t prevWidth = 0, prevHeight = 0;

		unsigned long m_Window = 0;
		int screen = 0;

		XEvent event;

		std::unordered_map<uint32_t, Time> pressTimes;
		uint32_t lastPressed = UINT32_MAX;
	};
}

#endif //__linux__