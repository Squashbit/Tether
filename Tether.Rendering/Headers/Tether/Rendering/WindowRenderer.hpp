#pragma once

#include <Tether/Rendering/RenderAction.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT WindowRenderer
	{
	public:
		WindowRenderer() = default;
		virtual ~WindowRenderer() = 0;

		virtual Scope<RenderAction> StartRender() = 0;
	};
}