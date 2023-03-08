#include <iostream>

#include <Tether/Tether.hpp>

#include <algorithm>

using namespace Tether;

int main()
{
	using DisplayMode = Devices::Monitor::DisplayMode;

	Application& app = Application::Get();

	std::cout << "Monitor count: " << app.GetMonitorCount() << '\n';

	std::vector<Devices::Monitor> monitors = app.GetMonitors();

	std::cout << "Monitors:\n";
	for (size_t i = 0; i < monitors.size(); i++)
	{
		const Devices::Monitor& monitor = monitors[i];

		std::cout << "\t[" << i << "]:\n";
		std::cout << "\t\tx = " << monitor.GetX() << '\n';
		std::cout << "\t\ty = " << monitor.GetY() << '\n';
		std::cout << "\t\twidth = " << monitor.GetWidth() << '\n';
		std::cout << "\t\theight = " << monitor.GetHeight() << '\n';
		std::cout << "\t\tname = " << monitor.GetDeviceName() << '\n';
		std::cout << "\t\tname = " << monitor.GetInternalDeviceName() << '\n';

		DisplayMode currentMode = monitor.GetCurrentMode();
		std::cout << "\t\tcurrentMode.name = " << currentMode.name << '\n';
		std::cout << "\t\tcurrentMode.refreshRate = " << currentMode.refreshRate << '\n';
		std::cout << "\t\tcurrentMode.exactRefreshRate = " << currentMode.exactRefreshRate << '\n';
		std::cout << "\t\tcurrentMode.width = " << currentMode.width << '\n';
		std::cout << "\t\tcurrentMode.height = " << currentMode.height << '\n';

		std::cout << "\t\tdisplayModes = \n";
		const std::vector<DisplayMode>& displayModes = monitor.GetDisplayModes();
		for (size_t j = 0; j < std::min(displayModes.size(), 
			static_cast<size_t>(10)); j++)
		{
			const DisplayMode& mode = displayModes[j];

			std::cout << "\t\t[" << j << "]\n";
			std::cout << "\t\t\tname = " << mode.name << '\n';
			std::cout << "\t\t\trefreshRate = " << mode.refreshRate << '\n';
			std::cout << "\t\t\texactRefreshRate = " << mode.exactRefreshRate << '\n';
			std::cout << "\t\t\twidth = " << mode.width << '\n';
			std::cout << "\t\t\theight = " << mode.height << '\n';
		}

		if (displayModes.size() > 10)
			std::cout << "\t\t\t...\n";

		std::cout << "\t\tisPrimary = " << monitor.IsPrimary() << '\n';
	}

	return 0;
}