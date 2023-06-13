#pragma once

#include <Tether/Rendering/Resources/Resource.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace Tether::Rendering::Resources
{
	class TETHER_EXPORT Font : public Resource
	{
	public:
		virtual ~Font();
		TETHER_NO_COPY(Font);

		void SetSize(uint32_t fontSize);

		virtual void LoadCharactersFromString(const std::string& text) = 0;
	protected:
		Font(const std::string& fontPath);
		
		FT_Face m_FontFace = nullptr;
	private:
		virtual void OnSetSize(uint32_t newSize) {}
	};
}