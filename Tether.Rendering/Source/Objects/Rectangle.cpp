#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/UIRenderer.hpp>

using namespace Tether::Rendering::Objects;

Rectangle::Rectangle(UIRenderer* pRenderer)
	:
	Object(pRenderer)
{
	pRenderer->CreateObject(this);
}

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
