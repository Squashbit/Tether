#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/WindowUI.hpp>

namespace Tether::Rendering::Elements
{
	Element::Element(WindowUI& windowUI)
		:
		m_WindowUI(windowUI),
		m_Window(m_WindowUI.GetWindow()),
		m_Renderer(m_WindowUI.GetRenderer())
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

		ChangeTransform();
	}

	void Element::SetY(float y) 
	{
		if (m_Y == y)
			return;

		m_Y = y;

		ChangeTransform();
	}

	void Element::SetWidth(float width) 
	{
		if (m_Width == width)
			return;

		m_Width = width;

		ChangeTransform();
	}

	void Element::SetHeight(float height) 
	{
		if (m_Height == height)
			return;

		m_Height = height;

		ChangeTransform();
	}

	void Element::SetBorderSize(float borderSize)
	{
		m_BorderSize = borderSize;
		ChangeTransform();
	}

	void Element::SetBorderColor(Math::Vector4f color)
	{
		m_BorderColor = color;
		ChangeStyle();
	}

	void Element::SetColor(Math::Vector4f color)
	{
		m_Color = color;
		ChangeStyle();
	}

	void Element::SetBackgroundColor(Math::Vector4f backgroundColor)
	{
		m_BackgroundColor = backgroundColor;
		ChangeStyle();
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

	void Element::OnAdd()
	{
		for (Objects::Object& object : m_Objects)
			m_Renderer.AddObject(object);
	}

	void Element::OnRemove()
	{
		for (Objects::Object& object : m_Objects)
			m_Renderer.RemoveObject(object);
	}

	void Element::ChangeTransform()
	{
		UpdateTransform();
		m_WindowUI.Repaint(true);
	}

	void Element::ChangeStyle()
	{
		UpdateStyle();
		m_WindowUI.Repaint(true);
	}
}