#ifdef __linux__

#include <Tether/Devices/DeviceManager.hpp>
#include <Tether/SimpleWindow.hpp>
#include <Tether/Native.hpp>

#include <X11/extensions/Xrandr.h>

#include <cmath>
#include <stdint.h>

using namespace Tether::Devices;

#define winStorage (pWindow->storage)
#define appStorage (app->storage)

uint64_t DeviceManager::GetMonitorCount()
{
    int numMonitors;
    XRRGetMonitors(appStorage->display, winStorage->window, false, 
        &numMonitors);

    return numMonitors;
}

bool DeviceManager::GetMonitor(uint64_t index, Monitor* pMonitor)
{
    if (!pMonitor || !pWindow->IsInitialized())
        return false;

    unsigned long root = RootWindow(appStorage->display, appStorage->screen);
    XRRScreenResources* resources = XRRGetScreenResources(appStorage->display, 
        root);

    int numMonitors;
    XRRMonitorInfo* monitorInfos = XRRGetMonitors(
        appStorage->display, winStorage->window, false, 
        &numMonitors
    );

    if (index >= numMonitors)
        return false;
    
    XRRMonitorInfo monitorInfo = monitorInfos[index];

    pMonitor->modes.clear();
    for (uint64_t i = 0; i < monitorInfo.noutput; i++)
    {
        XRROutputInfo* outputInfo = XRRGetOutputInfo(appStorage->display, 
            resources,
            monitorInfo.outputs[i]
        );
        XRRCrtcInfo* info = XRRGetCrtcInfo(appStorage->display, resources, 
            outputInfo->crtc);
        
        for (uint64_t i2 = 0; i2 < outputInfo->nmode; i2++)
        {
            XRRModeInfo mode;
            for (uint64_t i3 = 0; i3 < resources->nmode; i3++)
                if (resources->modes[i3].id == outputInfo->modes[i2])
                    mode = resources->modes[i3];
            
            DisplayMode displayMode;
            displayMode.name = mode.name;
            displayMode.exactRefreshRate = mode.dotClock 
                    / ((double)mode.hTotal * mode.vTotal);
            displayMode.refreshRate = round(displayMode.exactRefreshRate);
            displayMode.width = mode.width;
            displayMode.height = mode.height;

            if (info->mode == mode.id)
                pMonitor->currentMode = displayMode;
            
            pMonitor->modes.push_back(displayMode);
        }

        XRRFreeCrtcInfo(info);
        XRRFreeOutputInfo(outputInfo);
    }

    char* name = XGetAtomName(appStorage->display, monitorInfo.name);
    
    pMonitor->index = index;
    pMonitor->width = monitorInfo.width;
    pMonitor->height = monitorInfo.height;
    pMonitor->name = name;
    pMonitor->primary = monitorInfo.primary;

    XFree(name);
    XRRFreeMonitors(monitorInfos);
    XRRFreeScreenResources(resources);
    
    return true;
}

#endif //__linux__