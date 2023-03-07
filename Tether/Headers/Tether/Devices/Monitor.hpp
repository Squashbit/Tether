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
			std::wstring name;
			uint64_t refreshRate = 0;
			double exactRefreshRate = 0.0;
			uint64_t width = 0;
			uint64_t height = 0;
		};

		Monitor(
			int x, int y, int width, int height, 
			std::wstring_view deviceName, bool primary, DisplayMode currentMode, 
			const std::vector<DisplayMode>& displayModes
		);

		const int GetX() const;
		const int GetY() const;
		const int GetWidth() const;
		const int GetHeight() const;
		const std::wstring_view GetDeviceName() const;
		const DisplayMode GetCurrentMode() const;
		const std::vector<DisplayMode>& GetDisplayModes() const;
		bool IsPrimary() const;
	protected:
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		bool primary = false;

		std::wstring_view name;
		
		DisplayMode currentMode;
		std::vector<DisplayMode> modes;
	};
}
