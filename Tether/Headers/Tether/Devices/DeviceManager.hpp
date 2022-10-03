#ifndef _TETHER_DEVICEMANAGER_HPP
#define _TETHER_DEVICEMANAGER_HPP

#include <Tether/Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Devices/Monitor.hpp>

namespace Tether
{
    class SimpleWindow;
}

namespace Tether::Devices
{
    // namespace Storage
    // {
    //     struct DeviceVarStorage;
    // }

    class TETHER_EXPORT DeviceManager
    {
        friend SimpleWindow;
    public:
        TETHER_NO_COPY(DeviceManager);

        DeviceManager();
        ~DeviceManager();

        bool Init(SimpleWindow* pWindow);

        uint64_t GetMonitorCount();
		// Not yet implemented on Windows
        bool GetMonitor(uint64_t index, Monitor* pMonitor);
    protected:
    private:
        SimpleWindow* pWindow;
        Application* app;

        // Storage::DeviceVarStorage* storage;
    };
}

#endif //_TETHER_DEVICEMANAGER_HPP