#include <Tether/Events/WindowResizeEvent.hpp>

using namespace Tether::Events;

WindowResizeEvent::WindowResizeEvent(
    uint64_t newWidth,
    uint64_t newHeight
)
    :
    newWidth(newWidth),
    newHeight(newHeight)
{}

uint64_t WindowResizeEvent::GetNewWidth()
{
    return newWidth;
}

uint64_t WindowResizeEvent::GetNewHeight()
{
    return newHeight;
}
