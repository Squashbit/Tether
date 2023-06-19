#include <Tether/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	BorderedElement::BorderedElement(WindowUIManager& windowUI, Element& element)
		:
		m_BorderWindowUI(windowUI),
		m_Element(element)
	{}

	void BorderedElement::SetBorderSize(float borderSize)
	{
		m_BorderSize = borderSize;
		m_Element.Repaint();
	}

	void BorderedElement::SetBorderColor(Math::Vector4f color)
	{
		m_BorderColor = color;
		m_Element.Repaint();
	}

	const float BorderedElement::GetBorderSize() const
	{
		return m_BorderSize;
	}

	void BorderedElement::DrawBorder(Renderer& renderer)
	{
		// Border rect
		renderer.FillRect(
			m_Element.GetX() - m_BorderSize,
			m_Element.GetY() - m_BorderSize,
			m_Element.GetWidth() + m_BorderSize * 2.0f,
			m_Element.GetHeight() + m_BorderSize * 2.0f,
			m_BorderColor
		);

		// Background rect
		renderer.FillRect(
			m_Element.GetX(),
			m_Element.GetY(),
			m_Element.GetWidth(),
			m_Element.GetHeight(),
			m_Element.GetBackgroundColor()
		);
	}
}