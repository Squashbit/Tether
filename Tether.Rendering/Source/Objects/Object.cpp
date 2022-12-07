#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <memory>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Object::Object(UIRenderer* pRenderer)
	:
	pRenderer(pRenderer)
{
	initialized = pRenderer != nullptr;
}

void Object::SetNative(Scope<ObjectNative>& native)
{
	this->pNative = std::move(native);
}

UIRenderer* Object::GetUIRenderer()
{
	return pRenderer;
}

ObjectNative* Object::GetNative()
{
	return pNative.get();
}
