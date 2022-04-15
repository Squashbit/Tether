#ifndef _TETHER_DEVICEMANAGER_HPP
#define _TETHER_DEVICEMANAGER_HPP

#include <Tether/Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Devices/Monitor.hpp>

namespace Tether
{
    class IWindow;
}

namespace Tether::Devices
{
    // namespace Storage
    // {
    //     struct DeviceVarStorage;
    // }

    class DeviceManager
    {
        friend IWindow;
    public:
        TETHER_NO_COPY(DeviceManager);

        DeviceManager();
        ~DeviceManager();

        bool Init(IWindow* pWindow);

        uint64_t GetMonitorCount();
		// Not yet implemented on Windows
        bool GetMonitor(uint64_t index, Monitor* pMonitor);
    protected:
    private:
        IWindow* pWindow;
        Application* app;

        // Storage::DeviceVarStorage* storage;
    };
}

#endif //_TETHER_DEVICEMANAGER_HPP