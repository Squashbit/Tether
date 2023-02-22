#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Devices/Monitor.hpp>

namespace Tether
{
    class Window;
}

namespace Tether::Devices
{
    class TETHER_EXPORT DeviceManager
    {
        friend Window;
    public:
        DeviceManager();
        
        uint64_t GetMonitorCount();
		// Not yet implemented on Windows
        bool GetMonitor(uint64_t index, Monitor* pMonitor);
    private:
        Application& app;
    };
}
