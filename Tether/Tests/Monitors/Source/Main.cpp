#include <iostream>

#include <Tether/Tether.hpp>

#include <locale>
#include <codecvt>

using namespace Tether;

int main()
{
	using DisplayMode = Devices::Monitor::DisplayMode;

	Application& app = Application::Get();

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wideConverter;

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
		std::cout << "\t\tname = " << wideConverter.to_bytes(
			monitor.GetDeviceName().data()) << '\n';

		DisplayMode currentMode = monitor.GetCurrentMode();
		std::cout << "\t\tcurrentMode.name = " << wideConverter.to_bytes(
			currentMode.name.data()) << '\n';
		std::cout << "\t\tcurrentMode.refreshRate = " << currentMode.refreshRate << '\n';
		std::cout << "\t\tcurrentMode.exactRefreshRate = " << currentMode.exactRefreshRate << '\n';
		std::cout << "\t\tcurrentMode.width = " << currentMode.width << '\n';
		std::cout << "\t\tcurrentMode.height = " << currentMode.height << '\n';

		std::cout << "\t\tdisplayModes = \n";
		const std::vector<DisplayMode>& displayModes = monitor.GetDisplayModes();
		for (size_t j = 0; j < displayModes.size(); j++)
		{
			const DisplayMode& mode = displayModes[j];

			std::cout << "\t\t[" << j << "]\n";
			std::cout << "\t\t\tname = " << wideConverter.to_bytes(
				mode.name.data()) << '\n';
			std::cout << "\t\t\trefreshRate = " << mode.refreshRate << '\n';
			std::cout << "\t\t\texactRefreshRate = " << mode.exactRefreshRate << '\n';
			std::cout << "\t\t\twidth = " << mode.width << '\n';
			std::cout << "\t\t\theight = " << mode.height << '\n';
		}

		std::cout << "\t\tisPrimary = " << monitor.IsPrimary() << '\n';
	}

	return 0;
}