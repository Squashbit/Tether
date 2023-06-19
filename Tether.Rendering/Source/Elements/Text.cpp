#include <Tether/Rendering/Elements/Text.hpp>

namespace Tether::Rendering::Elements
{
	Text::Text(WindowUIManager& windowUI)
		:
		Element(windowUI)
	{}

	void Text::SetText(std::string_view text)
	{
		m_Text = text;
		Repaint();
	}

	void Text::SetJustify(Justify justify)
	{
		m_Justify = justify;
		Repaint();
	}

	void Text::SetFont(Resources::Font& font)
	{
		m_pFont = &font;
		Repaint();
	}

	void Text::OnRender(Renderer& renderer)
	{
		if (!m_pFont)
			return;

		renderer.DrawText(m_X, m_Y, m_Text, *m_pFont, m_Color, 1.0f, m_Justify);
	}
}