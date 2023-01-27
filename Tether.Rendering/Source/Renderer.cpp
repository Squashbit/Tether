#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering
{
	Renderer::~Renderer()
	{
		ClearObjects();
	}

	void Renderer::AddObject(Objects::Object* pObject)
	{
		if (pObject->m_IsInRenderer || pObject->GetRenderer() != this)
			return;

		objects.push_back(pObject);

		pObject->m_IsInRenderer = true;
	}

	bool Renderer::RemoveObject(Objects::Object* pObject)
	{
		if (!pObject->m_IsInRenderer)
			return false;

		for (size_t i = 0; i < objects.size(); i++)
			if (objects[i] == pObject)
			{
				pObject->m_IsInRenderer = false;
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
			objects[i]->m_IsInRenderer = false;

		objects.clear();
	}

	const std::vector<Objects::Object*>& Renderer::GetObjects() const
	{
		return objects;
	}
}
