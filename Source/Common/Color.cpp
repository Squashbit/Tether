#include <Tether/Common/Color.hpp>

using namespace Tether;

Color::Color()
    :
    r(0), g(0), b(0)
{}

Color::Color(float color)
    :
    r(color), g(color), b(color)
{}

Color::Color(float r, float g, float b)
    :
    r(r), g(g), b(b)
{}

Color::Color(const Color& ref)
{
    r = ref.r;
    g = ref.g;
    b = ref.b;
}

uint64_t Color::ToRGB(float r,float g, float b)
{
    uint8_t rInt = (uint8_t)(r * 255);
    uint8_t gInt = (uint8_t)(g * 255);
    uint8_t bInt = (uint8_t)(b * 255);

    return bInt + (gInt << 8) + (rInt << 16);
}

uint64_t Color::GetRGB() const
{
    return ToRGB(r, g, b);
}

void Color::SetR(float r)
{
    this->r = r;
}

void Color::SetG(float g)
{
    this->g = g;
}

void Color::SetB(float b)
{
    this->b = b;
}

float Color::GetR() const
{
    return r;
}

float Color::GetG() const
{
    return g;
}

float Color::GetB() const
{
    return b;
}