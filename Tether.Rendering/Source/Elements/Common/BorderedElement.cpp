#include <Tether/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	BorderedElement::BorderedElement(WindowUI& windowUI, Element& element)
		:
		m_BorderWindowUI(windowUI),
		m_Element(element)
	{
		Renderer& renderer = windowUI.GetRenderer();

		m_BackgroundRect = renderer.CreateObject<Objects::Rectangle>();
		m_BorderRect = renderer.CreateObject<Objects::Rectangle>();
	}

	void BorderedElement::SetBorderSize(float borderSize)
	{
		m_BorderSize = borderSize;
		m_Element.ChangeTransform();
	}

	void BorderedElement::SetBorderColor(Math::Vector4f color)
	{
		m_BorderRect->SetColor(color);
		m_BorderWindowUI.Repaint(true);
	}

	void BorderedElement::UpdateBorderTransform()
	{
		m_BorderRect->SetX(m_Element.GetX() - m_BorderSize);
		m_BorderRect->SetY(m_Element.GetY() - m_BorderSize);
		m_BorderRect->SetWidth(m_Element.GetWidth() + m_BorderSize * 2.0f);
		m_BorderRect->SetHeight(m_Element.GetHeight() + m_BorderSize * 2.0f);

		m_BackgroundRect->SetX(m_Element.GetX());
		m_BackgroundRect->SetY(m_Element.GetY());
		m_BackgroundRect->SetWidth(m_Element.GetWidth());
		m_BackgroundRect->SetHeight(m_Element.GetHeight());
	}

	void BorderedElement::UpdateBorderStyle()
	{
		m_BackgroundRect->SetColor(m_Element.GetBackgroundColor());
	}

	const float BorderedElement::GetBorderSize() const
	{
		return m_BorderSize;
	}

	void BorderedElement::AddBorderObjects(
		std::vector<std::reference_wrapper<Objects::Object>>& objects)
	{
		objects.push_back(*m_BorderRect);
		objects.push_back(*m_BackgroundRect);
	}
}