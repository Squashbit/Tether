#include <Tether/Module/Rendering/Objects/Text.hpp>

namespace Tether::Rendering::Objects
{
	Text::Text(ObjectRenderer& objectRenderer)
		:
		Objects::Object(objectRenderer),
		m_Color(1.0f)
	{}

	void Text::SetText(const std::string& text)
	{
		m_Text = text;

		if (m_pFont)
			m_pFont->LoadCharactersFromString(m_Text);
	}

	void Text::SetFont(Resources::Font& font)
	{
		m_pFont = &font;
		m_pFont->LoadCharactersFromString(m_Text);
	}

	void Text::SetScale(float scale)
	{
		m_Scale = scale;
	}

	void Text::SetJustify(Justify justify)
	{
		m_Justify = justify;
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