#include <Tether/Module/Rendering/Objects/Object.hpp>

using namespace Tether::Rendering;
using namespace Tether::Rendering::Objects;

Object::Object(UIRenderer* pRenderer, Scope<ObjectRenderer> pObjectRenderer)
	:
	pRenderer(pRenderer),
	pObjectRenderer(std::move(pObjectRenderer))
{
	this->pObjectRenderer->OnObjectUpdate();
}

Object::Object(Object&& other) noexcept
	:
	pRenderer(other.pRenderer),
	pObjectRenderer(std::move(other.pObjectRenderer))
{

}

UIRenderer* Object::GetUIRenderer()
{
	return pRenderer;
}

ObjectRenderer* Object::GetObjectRenderer()
{
	return pObjectRenderer.get();
}
