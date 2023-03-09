#include <Tether/Events/WindowMoveEvent.hpp>

namespace Tether::Events
{
	WindowMoveEvent::WindowMoveEvent(
		int64_t x,
		int64_t y
	)
		:
		x(x),
		y(y)
	{}

	const int64_t WindowMoveEvent::GetX() const
{
		return x;
	}

	const int64_t WindowMoveEvent::GetY() const
{
		return y;
	}
}