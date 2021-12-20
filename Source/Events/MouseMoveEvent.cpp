#include <Link/Events/MouseMoveEvent.hpp>

using namespace Link::Events;

MouseMoveEvent::MouseMoveEvent(
    uint64_t rootX,
    uint64_t rootY,
    uint64_t relativeX,
    uint64_t relativeY,
    uint64_t oldRelativeX,
    uint64_t oldRelativeY,
    uint64_t oldRootX,
    uint64_t oldRootY
)
    :
    rootX(rootX),
    rootY(rootY),
    relativeX(relativeX),
    relativeY(relativeY),
    oldRelativeX(oldRelativeX),
    oldRelativeY(oldRelativeY),
    oldRootX(oldRootX),
    oldRootY(oldRootY)
{}

MouseMoveEvent::MouseMoveEvent(const MouseMoveEvent& ref)
{
    rootX = ref.rootX;
    rootY = ref.rootY;
    relativeX = ref.relativeX;
    relativeY = ref.relativeY;
    oldRelativeX = ref.oldRelativeX;
    oldRelativeY = ref.oldRelativeY;
    oldRootX = ref.oldRootX;
    oldRootY = ref.oldRootY;
}

uint64_t MouseMoveEvent::GetRootX()
{
    return rootX;
}

uint64_t MouseMoveEvent::GetRootY()
{
    return rootY;
}

uint64_t MouseMoveEvent::GetRelativeX()
{
    return relativeX;
}

uint64_t MouseMoveEvent::GetRelativeY()
{
    return relativeY;
}

uint64_t MouseMoveEvent::GetOldRelativeX()
{
    return oldRelativeX;
}

uint64_t MouseMoveEvent::GetOldRelativeY()
{
    return oldRelativeY;
}

uint64_t MouseMoveEvent::GetOldRootX()
{
    return oldRootX;
}

uint64_t MouseMoveEvent::GetOldRootY()
{
    return oldRootY;
}
