#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

namespace Tether::Rendering
{
	class WindowUI;
}

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Element
	{
	public:
		Element(WindowUI& windowUI);

		void SetColor(Math::Vector4f color);
		void SetBackgroundColor(Math::Vector4f backgroundColor);

		Math::Vector4f GetColor();
		Math::Vector4f GetBackgroundColor();
	protected:
		virtual void UpdateStyle() {};

		Math::Vector4f m_Color;
		Math::Vector4f m_BackgroundColor;

		WindowUI& m_WindowUI;
	};
}
