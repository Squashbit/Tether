#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	Rectangle::Rectangle(ObjectRenderer* pObjectRenderer)
		:
		Objects::Object(pObjectRenderer)
	{}

	void Rectangle::SetWidth(float value)
	{
		this->width = value;
	}

	void Rectangle::SetHeight(float value)
	{
		this->height = value;
	}

	void Rectangle::SetColor(Color color)
	{
		this->color = color;
	}

	float Rectangle::GetWidth()
	{
		return width;
	}

	float Rectangle::GetHeight()
	{
		return height;
	}

	Color Rectangle::GetColor()
	{
		return color;
	}
}
