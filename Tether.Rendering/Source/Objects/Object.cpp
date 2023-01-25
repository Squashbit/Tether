#include <Tether/Module/Rendering/Objects/Object.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Renderer* Object::GetRenderer()
{
	return m_pRenderer;
}

ObjectRenderer* Object::GetObjectRenderer()
{
	return m_pObjectRenderer;
}
