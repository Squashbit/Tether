#include <Tether/Input/MouseMoveInfo.hpp>

using namespace Tether::Input;

MouseMoveInfo::MouseMoveInfo(
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

MouseMoveInfo::MouseMoveInfo(const MouseMoveInfo& ref)
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

uint64_t MouseMoveInfo::GetRootX()
{
    return rootX;
}

uint64_t MouseMoveInfo::GetRootY()
{
    return rootY;
}

uint64_t MouseMoveInfo::GetRelativeX()
{
    return relativeX;
}

uint64_t MouseMoveInfo::GetRelativeY()
{
    return relativeY;
}

uint64_t MouseMoveInfo::GetOldRelativeX()
{
    return oldRelativeX;
}

uint64_t MouseMoveInfo::GetOldRelativeY()
{
    return oldRelativeY;
}

uint64_t MouseMoveInfo::GetOldRootX()
{
    return oldRootX;
}

uint64_t MouseMoveInfo::GetOldRootY()
{
    return oldRootY;
}
