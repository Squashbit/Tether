#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT Compositor
	{
	public:
		void SetClearColor(Math::Vector4f clearColor);
		virtual bool RenderFrame() = 0;
	protected:
		Math::Vector4f m_ClearColor;
	};
}