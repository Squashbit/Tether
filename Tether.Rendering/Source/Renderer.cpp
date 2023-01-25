#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering
{
	Renderer::Renderer()
	{

	}

	Renderer::~Renderer()
	{
		ClearObjects();
	}

	void Renderer::AddObject(Objects::Object* pObject)
	{
		if (HasObject(pObject) || pObject->GetRenderer() != this)
			return;

		objects.push_back(pObject);
	}

	bool Renderer::RemoveObject(Objects::Object* pObject)
	{
		for (size_t i = 0; i < objects.size(); i++)
			if (objects[i] == pObject)
			{
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
		objects.clear();
	}

	const std::vector<Objects::Object*>& Renderer::GetObjects() const
	{
		return objects;
	}
}
