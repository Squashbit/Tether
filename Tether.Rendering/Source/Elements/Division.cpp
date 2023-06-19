#include <Tether/Rendering/Elements/Division.hpp>

namespace Tether::Rendering::Elements
{
	Division::Division(WindowUIManager& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this)
	{}

	void Division::OnRender(Renderer& renderer)
	{
		renderer.FillRect(m_X, m_Y, m_Width, m_Height, m_BackgroundColor);
	}
}