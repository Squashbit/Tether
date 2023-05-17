#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>

namespace Tether::Rendering
{
	class TETHER_EXPORT FreetypeLibrary
	{
	public:
		~FreetypeLibrary();
		TETHER_NO_COPY(FreetypeLibrary);

		FT_Library GetFreetypeLibrary();

		static FreetypeLibrary& Get();
	private:
		FreetypeLibrary();

		FT_Library m_Ft;

		inline static Scope<FreetypeLibrary> m_Internal;
	};
}