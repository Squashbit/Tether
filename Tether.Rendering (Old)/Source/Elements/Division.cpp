#include <Tether/Module/Rendering/Elements/Division.hpp>

namespace Tether::Rendering::Elements
{
	Division::Division(WindowUI& windowUI)
		:
		Element(windowUI),
		m_BackgroundRect(m_Renderer.CreateObject<Objects::Rectangle>())
	{
		m_Objects.push_back(*m_BackgroundRect);
	}

	void Division::UpdateTransform()
	{
		m_BackgroundRect->SetX(m_X);
		m_BackgroundRect->SetY(m_Y);
		m_BackgroundRect->SetWidth(m_Width);
		m_BackgroundRect->SetHeight(m_Height);
	}

	void Division::UpdateStyle()
	{
		m_BackgroundRect->SetColor(m_BackgroundColor);
	}
}