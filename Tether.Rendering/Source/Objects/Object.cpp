#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <Tether/Module/Rendering/GraphicsContext.hpp>

namespace Tether::Rendering::Objects
{
	Object::Object(GraphicsContext& graphicsContext, ObjectRenderer& objectRenderer)
		:
		m_GraphicsContext(graphicsContext),
		m_ObjectRenderer(objectRenderer)
	{}

	Object::~Object() 
	{}

	void Object::SetX(float value)
	{
		this->x = value;
	}

	void Object::SetY(float value)
	{
		this->y = value;
	}

	void Object::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}

	float Object::GetX()
	{
		return x;
	}

	float Object::GetY()
	{
		return y;
	}

	bool Object::IsEnabled()
	{
		return m_Enabled;
	}

	ObjectRenderer& Object::GetObjectRenderer() const
	{
		return m_ObjectRenderer;
	}

	GraphicsContext& Object::GetGraphicsContext() const
	{
		return m_GraphicsContext;
	}
}
