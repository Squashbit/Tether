#include <Tether/Input/MouseClickInfo.hpp>

namespace Tether::Input
{
	MouseClickInfo::MouseClickInfo(
		int rootX,
		int rootY,
		int relativeX,
		int relativeY,
		ClickType type,
		bool pressed
	)
		:
		rootX(rootX),
		rootY(rootY),
		relativeX(relativeX),
		relativeY(relativeY),
		type(type),
		pressed(pressed)
	{}

	int MouseClickInfo::GetRootX()
	{
		return rootX;
	}

	int MouseClickInfo::GetRootY()
	{
		return rootY;
	}

	int MouseClickInfo::GetRelativeX()
	{
		return relativeX;
	}

	int MouseClickInfo::GetRelativeY()
	{
		return relativeY;
	}

	MouseClickInfo::ClickType MouseClickInfo::GetClickType()
	{
		return type;
	}

	bool MouseClickInfo::IsPressed()
	{
		return pressed;
	}
}
