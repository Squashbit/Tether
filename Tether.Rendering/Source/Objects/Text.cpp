#include <Tether/Module/Rendering/Objects/Text.hpp>

namespace Tether::Rendering::Objects
{
	Text::Text(ObjectRenderer* pObjectRenderer)
		:
		Objects::Object(pObjectRenderer),
		m_Color(1.0f)
	{}

	void Text::SetText(const std::string& text)
	{
		m_Text = text;

		if (m_pFont)
			m_pFont->LoadCharactersFromString(m_Text);
	}

	void Text::SetFont(Resources::Font* pFont)
	{
		m_pFont = pFont;

		if (m_pFont)
			m_pFont->LoadCharactersFromString(m_Text);
	}

	void Text::SetScale(float scale)
	{
		m_Scale = scale;
	}

	void Text::SetColor(Math::Vector4f color)
	{
		m_Color = color;
	}

	Math::Vector4f Text::GetColor()
	{
		return m_Color;
	}
}