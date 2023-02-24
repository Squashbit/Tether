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
		m_X = x;
		UpdateTransform();
	}

	void Element::SetY(float y) 
	{
		m_Y = y;
		UpdateTransform();
	}

	void Element::SetWidth(float width) 
	{
		m_Width = width;
		UpdateTransform();
	}

	void Element::SetHeight(float height) 
	{
		m_Height = height;
		UpdateTransform();
	}

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
}