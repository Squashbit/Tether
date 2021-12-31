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

void Control::SetForegroundColor(Color color)
{
    foreground = color;
}

void Control::SetBackgroundColor(Color color)
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

Tether::Color Control::GetForegroundColor() const
{
    return foreground;
}

Tether::Color Control::GetBackgroundColor() const
{
    return background;
}
