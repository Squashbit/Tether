#include <Tether/Module/Rendering/FreetypeLibrary.hpp>
#include <stdexcept>

namespace Tether::Rendering
{
	FreetypeLibrary::FreetypeLibrary()
	{
		if (FT_Init_FreeType(&m_Ft))
			throw std::runtime_error("Failed to initialize Freetype");
	}

	FreetypeLibrary::~FreetypeLibrary()
	{
		FT_Done_FreeType(m_Ft);
	}

	FT_Library FreetypeLibrary::GetFreetypeLibrary()
	{
		return m_Ft;
	}

	FreetypeLibrary& FreetypeLibrary::Get()
	{
		if (!m_Internal)
			m_Internal = Scope<FreetypeLibrary>(new FreetypeLibrary());

		return *m_Internal;
	}
}