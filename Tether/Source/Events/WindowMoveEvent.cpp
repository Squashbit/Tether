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

int64_t WindowMoveEvent::GetX()
{
    return x;
}

int64_t WindowMoveEvent::GetY()
{
    return y;
}
