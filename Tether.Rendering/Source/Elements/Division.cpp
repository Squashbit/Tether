#include <Tether/Rendering/Elements/Division.hpp>

namespace Tether::Rendering::Elements
{
	Division::Division(WindowUIManager& windowUI)
		:
		Element(windowUI),
		BorderedElement(windowUI, *this)
	{
		AddBorderObjects(m_Objects);
	}

	void Division::UpdateTransform()
	{
		UpdateBorderTransform();
	}

	void Division::UpdateStyle()
	{
		UpdateBorderStyle();
	}
}