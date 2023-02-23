#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	Image::Image(ObjectRenderer* pObjectRenderer)
		:
		Objects::Object(pObjectRenderer)
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
