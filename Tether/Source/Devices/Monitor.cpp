#include <Tether/Devices/Monitor.hpp>

namespace Tether::Devices
{
    Monitor::Monitor(
        int x, int y, int width, int height, 
		std::wstring_view deviceName, bool primary, DisplayMode currentMode,
		const std::vector<DisplayMode>& displayModes
    )
        :
        x(x),
        y(y),
        width(width),
        height(height),
        primary(primary),
        name(deviceName),
        currentMode(currentMode),
        modes(displayModes)
    {}

    const int Monitor::GetX() const
    {
        return x;
    }

    const int Monitor::GetY() const
    {
        return y;
    }

    const int Monitor::GetWidth() const
    {
        return width;
    }

    const int Monitor::GetHeight() const
    {
        return height;
    }

    bool Monitor::IsPrimary() const
    {
        return primary;
    }

    const std::wstring_view Monitor::GetDeviceName() const
    {
        return name;
    }

    const Monitor::DisplayMode Monitor::GetCurrentMode() const
    {
        return currentMode;
    }

    const std::vector<Monitor::DisplayMode>& Monitor::GetDisplayModes() const
    {
        return modes;
    }
}
