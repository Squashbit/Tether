#include <Tether/Module/Rendering/Objects/Object.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Object::Object(Renderer* pRenderer)
	:
	pRenderer(pRenderer)
{}

Renderer* Object::GetUIRenderer()
{
	return pRenderer;
}

ObjectRenderer* Object::GetObjectRenderer()
{
	return pObjectRenderer;
}
