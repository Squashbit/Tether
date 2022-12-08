#pragma once

#include <Tether/SimpleWindow.hpp>

namespace Tether::Native
{
	class TETHER_EXPORT SimpleWindowNative
	{
	public:
		SimpleWindowNative(SimpleWindow* pWindow);
		virtual ~SimpleWindowNative() = default;

		virtual bool OnInit(int width, int height, const char* title, bool visible) = 0;

		virtual bool Run() = 0;

		virtual bool IsVisible() = 0;
		virtual void SetVisible(bool visibility) = 0;
		virtual void SetRawInputEnabled(bool enabled) = 0;
		virtual void SetCursorMode(CursorMode mode) = 0;
		virtual void SetCursorPos(int x, int y) = 0;
		virtual void SetCursorRootPos(int x, int y) = 0;
		virtual void SetX(int x) = 0;
		virtual void SetY(int y) = 0;
		virtual void SetPosition(int x, int y) = 0;
		virtual void SetWidth(int width) = 0;
		virtual void SetHeight(int height) = 0;
		virtual void SetSize(int width, int height) = 0;
		virtual void SetTitle(const char* title) = 0;
		virtual void SetBoundsEnabled(bool enabled) = 0;
		virtual void SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight) = 0;
		virtual void SetDecorated(bool enabled) = 0;
		virtual void SetResizable(bool resizable) = 0;
		virtual void SetClosable(bool closable) = 0;
		virtual void SetButtonStyleBitmask(uint8_t mask) = 0;
		virtual void SetMaximized(bool maximized) = 0;
		virtual void SetFullscreen(bool fullscreen, FullscreenSettings* settings,
			Devices::Monitor* monitor) = 0;
		virtual void PollEvents() = 0;
		virtual int GetX() = 0;
		virtual int GetY() = 0;
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;
		virtual int GetMouseX() = 0;
		virtual int GetMouseY() = 0;
		virtual int GetRelativeMouseX() = 0;
		virtual int GetRelativeMouseY() = 0;
		virtual bool IsFocused() = 0;

		virtual void OnDispose() {}
	protected:
		SimpleWindow* pWindow = nullptr;
	};
}