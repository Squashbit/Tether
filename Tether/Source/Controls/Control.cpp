#include <Tether/Controls/Control.hpp>

using namespace Tether::Controls;

void Control::SetX(float x)
{
    this->x = x;
}

void Control::SetY(float y)
{
    this->y = y;
}

void Control::SetWidth(float width)
{
    this->width = width;
}

void Control::SetHeight(float height)
{
    this->height = height;
}

void Control::SetForegroundColor(Math::Vector4f color)
{
    foreground = color;
}

void Control::SetBackgroundColor(Math::Vector4f color)
{
    background = color;
}

float Control::GetX() const
{
    return x;
}

float Control::GetY() const
{
    return y;
}

float Control::GetWidth() const
{
    return width;
}

float Control::GetHeight() const
{
    return height;
}

Tether::Math::Vector4f Control::GetForegroundColor() const
{
    return foreground;
}

Tether::Math::Vector4f Control::GetBackgroundColor() const
{
    return background;
}
