#pragma once

#include <Tether/Math/Types.hpp>

namespace Tether::Rendering::Vulkan
{
	struct Transform
	{
		Math::Vector2f position;
		alignas(16) Math::Vector2f scale;
	};
}