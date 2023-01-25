#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	void Image::SetX(float value)
	{
		this->x = value;
	}

	void Image::SetY(float value)
	{
		this->y = value;
	}

	void Image::SetWidth(float value)
	{
		this->width = value;
	}

	void Image::SetHeight(float value)
	{
		this->height = value;
	}

	float Image::GetX()
	{
		return x;
	}

	float Image::GetY()
	{
		return y;
	}

	float Image::GetWidth()
	{
		return width;
	}

	float Image::GetHeight()
	{
		return height;
	}
}
