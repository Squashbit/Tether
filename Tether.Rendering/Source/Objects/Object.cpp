#include <Tether/Module/Rendering/Objects/Object.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Object::Object(UIRenderer* pRenderer)
	:
	pRenderer(pRenderer)
{}

UIRenderer* Object::GetUIRenderer()
{
	return pRenderer;
}

ObjectRenderer* Object::GetObjectRenderer()
{
	return pObjectRenderer;
}
