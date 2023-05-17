#include <Tether/Rendering/Elements/Text.hpp>

namespace Tether::Rendering::Elements
{
	Text::Text(WindowUI& windowUI)
		:
		Element(windowUI),
		m_TextObject(m_Renderer.CreateObject<Objects::Text>())
	{
		m_Objects.push_back(*m_TextObject);
	}

	void Text::SetText(std::string_view text)
	{
		m_TextObject->SetText(text.data());
		m_WindowUI.Repaint(true);
	}

	void Text::SetJustify(Justify justify)
	{
		m_TextObject->SetJustify(justify);
	}

	void Text::SetFont(Resources::Font& font)
	{
		m_TextObject->SetFont(font);
		m_WindowUI.Repaint(true);
	}

	void Text::UpdateTransform()
	{
		m_TextObject->SetX(m_X);
		m_TextObject->SetY(m_Y);
	}

	void Text::UpdateStyle()
	{
		m_TextObject->SetColor(m_Color);
	}
}