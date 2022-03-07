#include <Tether/Events/RawMouseMoveEvent.hpp>

using namespace Tether::Events;

RawMouseMoveEvent::RawMouseMoveEvent(
    uint64_t rawX,
    uint64_t rawY
)
    :
    rawX(rawX),
    rawY(rawY)
{}

RawMouseMoveEvent::RawMouseMoveEvent(const RawMouseMoveEvent& ref)
{
    rawX = ref.rawX;
    rawY = ref.rawY;
}

int64_t RawMouseMoveEvent::GetRawX()
{
    return rawX;
}

int64_t RawMouseMoveEvent::GetRawY()
{
    return rawY;
}
