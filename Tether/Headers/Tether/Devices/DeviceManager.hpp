#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Devices/Monitor.hpp>

namespace Tether
{
    class SimpleWindow;
}

namespace Tether::Devices
{
    class TETHER_EXPORT DeviceManager
    {
        friend SimpleWindow;
    public:
        TETHER_NO_COPY(DeviceManager);

        DeviceManager(SimpleWindow* pWindow);
        
        uint64_t GetMonitorCount();
		// Not yet implemented on Windows
        bool GetMonitor(uint64_t index, Monitor* pMonitor);
    private:
        SimpleWindow* pWindow = nullptr;
        Application* app = nullptr;
    };
}
