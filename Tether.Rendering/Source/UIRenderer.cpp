#include <Tether/Module/Rendering/UIRenderer.hpp>

using namespace Tether::Rendering;

UIRenderer::UIRenderer()
{
	initialized = true;
}

UIRenderer::~UIRenderer()
{
	Dispose();
}

void UIRenderer::AddObject(Objects::Object* pObject)
{
	if (!pObject->IsInitialized() || pObject->GetUIRenderer() != this)
		return;

	objects.push_back(pObject);
	OnObjectAdd(pObject);
}

bool UIRenderer::RemoveObject(Objects::Object* pObject)
{
	if (!pObject->IsInitialized() || pObject->GetUIRenderer() != this)
		return false;

	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
		{
			OnObjectRemove(pObject);
			objects.erase(objects.begin() + i);

			return true;
		}

	return false;
}

bool UIRenderer::HasObject(Objects::Object* pObject)
{
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
			return true;

	return false;
}

void UIRenderer::ClearObjects()
{
	for (size_t i = 0; i < objects.size(); i++)
		OnObjectRemove(objects[i]);

	objects.clear();
}

const std::vector<Objects::Object*>& UIRenderer::GetObjects() const
{
	return objects;
}

void UIRenderer::OnDispose()
{
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->Dispose();
	objects.clear();

	OnRendererDispose();
}
