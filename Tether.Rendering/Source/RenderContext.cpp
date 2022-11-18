#include <Tether/Module/Rendering/RenderContext.hpp>

using namespace Tether::Rendering;

bool RenderContext::Init(IRenderContextNative* pNative)
{
	if (initialized || !pNative)
		return false;

	this->pNative = pNative;
	this->pNative->pRenderContext = this;

	initialized = true;
	return true;
}

void RenderContext::Add(Objects::Object* pObject)
{
	objects.push_back(pObject);
	pNative->OnObjectAdd(pObject);
}

bool RenderContext::Remove(Objects::Object* pObject)
{
	pNative->OnObjectRemove(pObject);

	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
		{
			objects.erase(objects.begin() + i);
			return true;
		}

	return false;
}

bool RenderContext::HasObject(Objects::Object* pObject)
{
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
			return true;

	return false;
}

bool RenderContext::RenderFrame()
{
	return pNative->RenderFrame();
}

IRenderContextNative* const RenderContext::GetNative() const
{
	return pNative;
}

std::vector<Objects::Object*>* RenderContext::GetObjects()
{
	return &objects;
}

void RenderContext::OnDispose()
{
	for (size_t i = 0; i < objects.size(); i++)
		objects[i]->Dispose();

	pNative->Dispose();

	objects.clear();
}
