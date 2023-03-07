#pragma once

#include <Tether/Application.hpp>

namespace Tether::Platform
{
	class TETHER_EXPORT Win32Application : public Application
	{
	private:
		size_t GetMonitorCount() override;
		std::vector<Devices::Monitor> GetMonitors() override;

		void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes) override;
	};
}