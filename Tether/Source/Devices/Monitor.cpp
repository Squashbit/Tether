#include <Tether/Devices/Monitor.hpp>

using namespace Tether::Devices;

int64_t Monitor::GetX() const
{
    return x;
}

int64_t Monitor::GetY() const
{
    return y;
}

uint64_t Monitor::GetIndex() const
{
    return index;
}

uint64_t Monitor::GetWidth() const
{
    return width;
}

uint64_t Monitor::GetHeight() const
{
    return height;
}

bool Monitor::IsPrimary() const
{
    return primary;
}

std::string Monitor::GetDeviceName() const
{
    return name;
}

DisplayMode Monitor::GetCurrentMode() const
{
    return currentMode;
}

DisplayMode* Monitor::GetDisplayModes() const
{
    return const_cast<DisplayMode*>(modes.data());
}