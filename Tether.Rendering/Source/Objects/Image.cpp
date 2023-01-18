#include <Tether/Module/Rendering/Objects/Image.hpp>
#include <Tether/Module/Rendering/Renderer.hpp>

namespace Tether::Rendering::Objects
{
	Image::Image(Renderer* pRenderer)
		:
		Object(pRenderer)
	{
		
	}

	void Image::SetX(float value)
	{
		this->x = value;
		pObjectRenderer->OnObjectUpdate();
	}

	void Image::SetY(float value)
	{
		this->y = value;
		pObjectRenderer->OnObjectUpdate();
	}

	void Image::SetWidth(float value)
	{
		this->width = value;
		pObjectRenderer->OnObjectUpdate();
	}

	void Image::SetHeight(float value)
	{
		this->height = value;
		pObjectRenderer->OnObjectUpdate();
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
