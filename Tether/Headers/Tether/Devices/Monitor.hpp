#pragma once

#include <Tether/Common/Defs.hpp>

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>

namespace Tether::Devices
{
	class TETHER_EXPORT Monitor
	{
	public:
		struct DisplayMode
		{
			DisplayMode(
				std::string name,
				uint64_t refreshRate,
				double exactRefreshRate,
				uint64_t width,
				uint64_t height
			)
				:
				name(name),
				refreshRate(refreshRate),
				exactRefreshRate(exactRefreshRate),
				width(width),
				height(height)
			{}

			const std::string name;
			const uint64_t refreshRate = 0;
			const double exactRefreshRate = 0.0;
			const uint64_t width = 0;
			const uint64_t height = 0;
		};

		Monitor(
			int x, int y, int width, int height, 
			std::string_view deviceName, 
			std::string_view internalDeviceName, 
			bool primary, DisplayMode currentMode,
			const std::vector<DisplayMode>& displayModes,
			int index = -1
		);

		const int GetX() const;
		const int GetY() const;
		const int GetWidth() const;
		const int GetHeight() const;
		const int GetIndex() const; // Unused on Windows
		const std::string GetDeviceName() const;
		const std::string GetInternalDeviceName() const;
		const DisplayMode GetCurrentMode() const;
		const std::vector<DisplayMode>& GetDisplayModes() const;
		bool IsPrimary() const;
	protected:
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		int index = 0;

		bool primary = false;

		std::string name;
		std::string internalName;
		
		DisplayMode currentMode;
		std::vector<DisplayMode> modes;
	};
}
