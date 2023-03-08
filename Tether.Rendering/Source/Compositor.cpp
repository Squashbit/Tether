#include <Tether/Module/Rendering/Compositor.hpp>

namespace Tether::Rendering
{
	void Compositor::SetClearColor(Math::Vector4f clearColor)
	{
		m_ClearColor = clearColor;
	}
}