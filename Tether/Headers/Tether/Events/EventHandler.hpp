#ifndef _TETHER_EVENTHANDLER_HPP
#define _TETHER_EVENTHANDLER_HPP

#include <Tether/Common/Defs.hpp>

#include <Tether/Events/WindowClosingEvent.hpp>
#include <Tether/Events/WindowRepaintEvent.hpp>
#include <Tether/Events/WindowResizeEvent.hpp>
#include <Tether/Events/WindowErrorEvent.hpp>
#include <Tether/Events/WindowMoveEvent.hpp>

#include <vector>

namespace Tether
{
	class SimpleWindow;
}

namespace Tether::Events
{
	class TETHER_EXPORT EventHandler
	{
		friend SimpleWindow;
	public:
		~EventHandler();

		virtual void OnWindowClosing(WindowClosingEvent event) {}
		virtual void OnWindowRepaint(WindowRepaintEvent event) {}
		virtual void OnWindowResize(WindowResizeEvent event) {}
		virtual void OnWindowMove(WindowMoveEvent event) {}
		virtual void OnWindowError(WindowErrorEvent event) {}
	protected:
		void OnAdd(SimpleWindow* pWindow);
		void OnRemove(SimpleWindow* pWindow);
	private:
		std::vector<SimpleWindow*> windows;
	};
}

#endif //_TETHER_EVENTHANDLER_HPP