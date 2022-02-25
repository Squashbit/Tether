#ifndef _TETHER_EVENTHANDLER_HPP
#define _TETHER_EVENTHANDLER_HPP

#include <Tether/Events/MouseMoveEvent.hpp>
#include <Tether/Events/WindowClosingEvent.hpp>
#include <Tether/Events/WindowRepaintEvent.hpp>
#include <Tether/Events/WindowResizeEvent.hpp>
#include <Tether/Events/WindowErrorEvent.hpp>

namespace Tether::Events
{
	class EventHandler
	{
	public:
		virtual void OnMouseMove(MouseMoveEvent event) {}
		virtual void OnWindowClosing(WindowClosingEvent event) {}
		virtual void OnWindowRepaint(WindowRepaintEvent event) {}
		virtual void OnWindowResize(WindowResizeEvent event) {}
		virtual void OnWindowError(WindowErrorEvent event) {}
	};
}

#endif //_TETHER_EVENTHANDLER_HPP