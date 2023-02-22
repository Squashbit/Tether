#include <Tether/Module/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	Element::Element(WindowUI& windowUI)
		:
		m_WindowUI(windowUI)
	{}

	void Element::SetColor(Math::Vector4f color)
	{
		m_Color = color;
		UpdateStyle();
	}

	void Element::SetBackgroundColor(Math::Vector4f backgroundColor)
	{
		m_BackgroundColor = backgroundColor;
		UpdateStyle();
	}

	Math::Vector4f Element::GetColor()
	{
		return m_Color;
	}

	Math::Vector4f Element::GetBackgroundColor()
	{
		return m_BackgroundColor;
	}
}