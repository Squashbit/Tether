#include <Tether/Module/Rendering/Objects/Image.hpp>

namespace Tether::Rendering::Objects
{
	Image::Image(ObjectRenderer& objectRenderer)
		:
		Objects::Object(objectRenderer)
	{}

	void Image::SetWidth(float value)
	{
		this->width = value;
	}

	void Image::SetHeight(float value)
	{
		this->height = value;
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
