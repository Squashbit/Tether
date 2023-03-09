#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Events/WindowClosingEvent.hpp>
#include <Tether/Events/WindowRepaintEvent.hpp>
#include <Tether/Events/WindowResizeEvent.hpp>
#include <Tether/Events/WindowMoveEvent.hpp>

#include <vector>

namespace Tether::Events
{
	class TETHER_EXPORT EventHandler
	{
	public:
		virtual void OnWindowClosing() {}
		virtual void OnWindowRepaint() {}
		virtual void OnWindowResize(const WindowResizeEvent& event) {}
		virtual void OnWindowMove(const WindowMoveEvent& event) {}
	};
}
