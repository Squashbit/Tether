#pragma once

#include <Tether/Module/Rendering/Resources/Resource.hpp>

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
	protected:
		Font(const std::string& fontPath);
	private:
		FT_Face m_FontFace = nullptr;
	};
}