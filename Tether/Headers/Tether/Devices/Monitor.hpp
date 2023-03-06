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
			std::string name;
			uint64_t refreshRate;
			double exactRefreshRate;
			uint64_t width;
			uint64_t height;
		};

		Monitor(
			size_t index, int x, int y, int width, int height, 
			std::string_view deviceName, bool primary, DisplayMode currentMode, 
			const std::vector<DisplayMode>& displayModes
		);

		const size_t GetIndex() const;
		const int GetX() const;
		const int GetY() const;
		const int GetWidth() const;
		const int GetHeight() const;
		const std::string GetDeviceName() const;
		const DisplayMode GetCurrentMode() const;
		const std::vector<DisplayMode>& GetDisplayModes() const;
		bool IsPrimary() const;
	protected:
		size_t index = 0;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		bool primary = false;

		std::string name = "";
		
		DisplayMode currentMode;
		std::vector<DisplayMode> modes;
	};
}
