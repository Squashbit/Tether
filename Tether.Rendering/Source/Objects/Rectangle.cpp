#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	void Rectangle::SetX(float value)
	{
		this->x = value;
	}

	void Rectangle::SetY(float value)
	{
		this->y = value;
	}

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

	float Rectangle::GetX()
	{
		return x;
	}

	float Rectangle::GetY()
	{
		return y;
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
