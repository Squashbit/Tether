#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	Rectangle::Rectangle(ObjectRenderer& objectRenderer)
		:
		Objects::Object(objectRenderer),
		color(1.0f)
	{}

	void Rectangle::SetWidth(float value)
	{
		this->width = value;
	}

	void Rectangle::SetHeight(float value)
	{
		this->height = value;
	}

	void Rectangle::SetColor(Math::Vector4f color)
	{
		this->color = color;
	}

	float Rectangle::GetWidth() const
	{
		return width;
	}

	float Rectangle::GetHeight() const
	{
		return height;
	}

	Math::Vector4f Rectangle::GetColor()
	{
		return color;
	}
}
