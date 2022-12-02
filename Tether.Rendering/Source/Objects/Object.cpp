#include <Tether/Module/Rendering/Objects/Object.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Object::Object(UIRenderer* pRenderer)
	:
	pRenderer(pRenderer)
{
	initialized = pRenderer != nullptr;
}

void Object::SetNative(ObjectNative* pNative)
{
	this->pNative = std::unique_ptr<ObjectNative>(pNative);
}

UIRenderer* Object::GetUIRenderer()
{
	return pRenderer;
}

ObjectNative* Object::GetNative()
{
	return pNative.get();
}
