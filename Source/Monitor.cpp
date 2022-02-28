#include <Tether/Monitor.hpp>

using namespace Tether;

Monitor::Monitor(const Monitor& ref)
{
    index = ref.index;
    width = ref.width;
    height = ref.height;
    currentMode = ref.currentMode;
    name = ref.name;

    for (DisplayMode mode : ref.modes)
        modes.push_back(mode);
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