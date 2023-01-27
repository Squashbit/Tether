#include <Tether/Module/Rendering/Resources/Font.hpp>
#include <Tether/Module/Rendering/FreetypeLibrary.hpp>
#include <stdexcept>

namespace Tether::Rendering::Resources
{
	Font::Font(const std::string& fontPath)
	{
		if (FT_New_Face(FreetypeLibrary::Get().GetFreetypeLibrary(), fontPath.c_str(),
			0, &m_FontFace))
			throw std::runtime_error("Failed to create font face");

		SetSize(18);
	}

	Font::~Font()
	{
		FT_Done_Face(m_FontFace);
	}

	void Font::SetSize(uint32_t fontSize)
	{
		FT_Set_Pixel_Sizes(m_FontFace, 0, fontSize);
	}
}