#ifndef _TETHER_MONITOR_HPP
#define _TETHER_MONITOR_HPP

#include <Tether/Common/Defs.hpp>

#include <iostream>
#include <vector>

namespace Tether
{
    class SimpleWindow;
}

namespace Tether::Devices
{
    struct DisplayMode
    {
        std::string name;
        uint64_t refreshRate;
        double exactRefreshRate;
        uint64_t width;
        uint64_t height;
    };

    class TETHER_EXPORT Monitor
    {
        friend SimpleWindow;
        friend class DeviceManager;
    public:
        Monitor() {}
		Monitor(const Monitor& ref);
            // bew work
        uint64_t GetIndex() const;
        int64_t GetX() const;
        int64_t GetY() const;
        uint64_t GetWidth() const;
        uint64_t GetHeight() const;
        std::string GetDeviceName() const;
        DisplayMode GetCurrentMode() const;
        DisplayMode* GetDisplayModes() const;
        bool IsPrimary() const;
    protected:
        uint64_t index = 0;
        int64_t x = 0;
        int64_t y = 0;
        uint64_t width = 0;
        uint64_t height = 0;

        bool primary = false;

        std::string name = "";
        
        DisplayMode currentMode;
        std::vector<DisplayMode> modes;
    };
}

#endif //_TETHER_MONITOR_HPP