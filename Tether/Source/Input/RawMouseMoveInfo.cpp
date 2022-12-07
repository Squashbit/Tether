#include <Tether/Input/RawMouseMoveInfo.hpp>

using namespace Tether::Input;

RawMouseMoveInfo::RawMouseMoveInfo(uint64_t rawX, uint64_t rawY)
	:
	rawX(rawX),
	rawY(rawY)
{}

int64_t RawMouseMoveInfo::GetRawX()
{
	return rawX;
}

int64_t RawMouseMoveInfo::GetRawY()
{
	return rawY;
}
