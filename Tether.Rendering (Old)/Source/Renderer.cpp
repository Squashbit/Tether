#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering
{
	Renderer::~Renderer()
	{
		ClearObjects();
	}

	void Renderer::AddObject(Objects::Object& object)
	{
		if (object.m_IsInRenderer || object.GetRenderer() != this)
			return;

		objects.push_back(&object);

		object.m_IsInRenderer = true;
	}

	bool Renderer::RemoveObject(Objects::Object& object)
	{
		if (!object.m_IsInRenderer)
			return false;

		for (size_t i = 0; i < objects.size(); i++)
			if (objects[i] == &object)
			{
				object.m_IsInRenderer = false;
				objects.erase(objects.begin() + i);

				return true;
			}

		return false;
	}

	bool Renderer::HasObject(Objects::Object& object)
	{
		return object.m_IsInRenderer && object.GetRenderer() == this;
	}

	void Renderer::ClearObjects()
	{
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->m_IsInRenderer = false;

		objects.clear();
	}

	void Renderer::SetClearColor(const Math::Vector3f& clearColor)
	{
		m_ClearColor = clearColor;
	}

	const std::vector<Objects::Object*>& Renderer::GetObjects() const
	{
		return objects;
	}
}
