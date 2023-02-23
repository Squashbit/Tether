#include <Tether/Module/Rendering/Elements/Button.hpp>

namespace Tether::Rendering::Elements
{
	Button::Button(WindowUI& windowUI)
		:
		Element(windowUI),
		m_BackgroundRect(m_Renderer.CreateObject<Objects::Rectangle>())
	{
		m_Objects.push_back(*m_BackgroundRect);
	}

	void Button::UpdateTransform()
	{
		m_BackgroundRect->SetX(m_X);
		m_BackgroundRect->SetY(m_Y);
		m_BackgroundRect->SetWidth(m_Width);
		m_BackgroundRect->SetHeight(m_Height);
	}

	void Button::UpdateStyle()
	{
		m_BackgroundRect->SetColor(m_BackgroundColor);
	}
}