#include <Tether/Events/WindowResizeEvent.hpp>

namespace Tether::Events
{
	WindowResizeEvent::WindowResizeEvent(
		uint64_t newWidth,
		uint64_t newHeight
	)
		:
		newWidth(newWidth),
		newHeight(newHeight)
	{}

	const uint64_t WindowResizeEvent::GetNewWidth() const
	{
		return newWidth;
	}

	const uint64_t WindowResizeEvent::GetNewHeight() const
	{
		return newHeight;
	}
}
