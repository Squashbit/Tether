#include <Tether/Module/Rendering/Renderer.hpp>

using namespace Tether::Rendering;

Renderer::Renderer()
{
	
}

Renderer::~Renderer()
{
	ClearObjects();
}

void Renderer::AddObject(Objects::Object* pObject)
{
	if (HasObject(pObject) || pObject->GetUIRenderer() != this)
		return;

	objects.push_back(pObject);
	OnObjectAdd(pObject);
}

bool Renderer::RemoveObject(Objects::Object* pObject)
{
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
		{
			OnObjectRemove(pObject);
			objects.erase(objects.begin() + i);

			return true;
		}

	return false;
}

bool Renderer::HasObject(Objects::Object* pObject)
{
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
			return true;

	return false;
}

void Renderer::ClearObjects()
{
	for (size_t i = 0; i < objects.size(); i++)
		OnObjectRemove(objects[i]);

	objects.clear();
}

const std::vector<Objects::Object*>& Renderer::GetObjects() const
{
	return objects;
}