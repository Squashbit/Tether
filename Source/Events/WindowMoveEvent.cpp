#include <Tether/Events/WindowMoveEvent.hpp>

using namespace Tether::Events;

WindowMoveEvent::WindowMoveEvent(
    int64_t x,
    int64_t y
)
    :
    x(x),
    y(y)
{}

WindowMoveEvent::WindowMoveEvent(const WindowMoveEvent& ref)
{
    x = ref.x;
    y = ref.y;
}

int64_t WindowMoveEvent::GetX()
{
    return x;
}

int64_t WindowMoveEvent::GetY()
{
    return y;
}
