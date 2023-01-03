#pragma once

#include <Tether/Module/Rendering/Math/Vector2.hpp>

namespace Tether::Rendering::Vulkan
{
	struct Transform
	{
		Vector2 position;
		Vector2 scale;
	};
}