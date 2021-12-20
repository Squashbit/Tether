#ifndef _LINK_EVENTHANDLER_HPP
#define _LINK_EVENTHANDLER_HPP

#include <Link/Events/MouseMoveEvent.hpp>
#include <Link/Events/WindowClosingEvent.hpp>
#include <Link/Events/WindowResizeEvent.hpp>

namespace Link::Events
{
	class EventHandler
	{
	public:
		virtual void OnMouseMove(MouseMoveEvent event) {}
		virtual void OnWindowClosing(WindowClosingEvent event) {}
		virtual void OnWindowResize(WindowResizeEvent event) {}
	};
}

#endif //_LINK_EVENTHANDLER_HPP