#include <Tether/Module/Rendering/Resources/Resource.hpp>

namespace Tether::Rendering::Resources
{
	Renderer* Resource::GetRenderer()
	{
		return m_pRenderer;
	}
}