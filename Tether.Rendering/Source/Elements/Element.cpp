#include <Tether/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	Element::Element(WindowUIManager& windowUI)
		:
		m_WindowUI(windowUI),
		m_Window(windowUI.GetWindow())
	{}

	Element::~Element()
	{
		if (m_IsInWindowUI)
			m_WindowUI.RemoveElement(*this);
	}

	void Element::SetX(float x) 
	{
		if (m_X == x)
			return;

		m_X = x;

		Repaint();
	}

	void Element::SetY(float y) 
	{
		if (m_Y == y)
			return;

		m_Y = y;

		Repaint();
	}

	void Element::SetWidth(float width) 
	{
		if (m_Width == width)
			return;

		m_Width = width;

		Repaint();
	}

	void Element::SetHeight(float height) 
	{
		if (m_Height == height)
			return;

		m_Height = height;

		Repaint();
	}
	
	void Element::SetColor(Math::Vector4f color)
	{
		m_Color = color;
		Repaint();
	}

	void Element::SetBackgroundColor(Math::Vector4f backgroundColor)
	{
		m_BackgroundColor = backgroundColor;
		Repaint();
	}

	float Element::GetX() const
	{
		return m_X;
	}

	float Element::GetY() const
	{
		return m_Y;
	}

	float Element::GetWidth() const
	{
		return m_Width;
	}

	float Element::GetHeight() const
	{
		return m_Height;
	}

	Math::Vector4f Element::GetColor() const
	{
		return m_Color;
	}

	Math::Vector4f Element::GetBackgroundColor() const
	{
		return m_BackgroundColor;
	}

	void Element::Repaint()
	{
		m_WindowUI.Repaint(true);
	}
}