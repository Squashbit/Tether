#ifdef _WIN32

#include <Tether/Devices/DeviceManager.hpp>
#include <Tether/SimpleWindow.hpp>
#include <Tether/Native.hpp>

#include <cmath>
#include <stdint.h>

using namespace Tether::Devices;

#define winStorage (pWindow->storage)

uint64_t DeviceManager::GetMonitorCount()
{
	return GetSystemMetrics(SM_CMONITORS);
}

static BOOL CALLBACK Tether_EnumerateMonitors(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
)
{
	std::vector<HMONITOR>* pMonitors = (std::vector<HMONITOR>*)dwData;
	pMonitors->push_back(hMonitor);

	return TRUE;
}

bool DeviceManager::GetMonitor(uint64_t index, Monitor* pMonitor)
{
	return false;

	/*std::vector<HMONITOR> monitors;
	EnumDisplayMonitors(NULL, NULL, Tether_EnumerateMonitors, (LPARAM)&monitors);

	if (index >= monitors.size())
		return false;

	EnumDisplayDevices()

	HMONITOR monitor = monitors[index];

	MONITORINFOEX monitorInfo{};
	GetMonitorInfo(monitor, &monitorInfo);

	pMonitor->name = monitorInfo.szDevice;

	RECT rect = monitorInfo.rcMonitor;
	pMonitor->x = rect.left;
	pMonitor->y = rect.top;
	pMonitor->width = rect.right;
	pMonitor->height = rect.bottom;

	uint64_t i = 0;
	DEVMODE devmode{};
	while (EnumDisplaySettings(NULL, i, &devmode))
	{
		DisplayMode mode{};
		mode.exactRefreshRate = devmode.dmDisplayFrequency;
		mode.refreshRate = devmode.dmDisplayFrequency;
		mode.name = (char*)devmode.dmDeviceName;

		pMonitor->modes.push_back(mode);

		i++;
	}*/
	
	// return true;
}

#endif //_WIN32