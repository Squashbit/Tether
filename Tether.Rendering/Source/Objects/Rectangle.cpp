#include <Tether/Module/Rendering/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/UIRenderer.hpp>

using namespace Tether::Rendering::Objects;

Rectangle::Rectangle(UIRenderer* pRenderer)
	:
	Object(pRenderer)
{}

void Rectangle::SetX(float value)
{
	this->x = value;
	pObjectRenderer->OnObjectUpdate();
}

void Rectangle::SetY(float value)
{
	this->y = value;
	pObjectRenderer->OnObjectUpdate();
}

void Rectangle::SetWidth(float value)
{
	this->width = value;
	pObjectRenderer->OnObjectUpdate();
}

void Rectangle::SetHeight(float value)
{
	this->height = value;
	pObjectRenderer->OnObjectUpdate();
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
