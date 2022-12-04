#include <Tether/Module/Rendering/UIRenderer.hpp>

using namespace Tether::Rendering;

UIRenderer::UIRenderer(UIRendererNative* pNative)
	:
	pNative(pNative)
{
	if (!pNative)
		return;

	this->pNative->pRenderer = this;

	initialized = true;
}

UIRenderer::~UIRenderer()
{
	Dispose();
}

void UIRenderer::AddObject(Objects::Object* pObject)
{
	if (!pObject->IsInitialized())
		return;

	objects.push_back(pObject);
	pNative->OnObjectAdd(pObject);
}

bool UIRenderer::RemoveObject(Objects::Object* pObject)
{
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
		{
			pNative->OnObjectRemove(pObject);
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
		pNative->OnObjectRemove(objects[i]);

	objects.clear();
}

bool UIRenderer::RenderFrame()
{
	return pNative->RenderFrame();
}

UIRendererNative* const UIRenderer::GetNative() const
{
	return pNative;
}

const std::vector<Objects::Object*>& UIRenderer::GetObjects() const
{
	return objects;
}

void UIRenderer::OnDispose()
{
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->Dispose();

	pNative->Dispose();

	objects.clear();
}
